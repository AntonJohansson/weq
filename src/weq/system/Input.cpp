#include <weq/system/Input.hpp>

#include <weq/ecs/System.hpp>
#include <weq/ecs/EventManager.hpp>

#include <weq/system/InputRaw.hpp>
#include <weq/system/InputTypes.hpp>
#include <weq/event/Input.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

//#include <spdlog/spdlog.h>
//#include <imgui/imgui.h>
//#include <imgui/imgui_impl_glfw_gl3.h>

//#include <GLFW/glfw3.h>
//
//#include <memory>
//#include <initializer_list>

// TODO read input from config file instead of hard coded.

namespace{
  unsigned int window_width;
  unsigned int window_height;
}

namespace weq::system{

namespace{
	event::ActiveInput _event;

	enum KeyType{ACTION, STATE, RANGE};
	struct KeyInfo{
		int key;
		KeyType type;
	};
	static std::unordered_map<std::string, int> key_map = {
		{"r", static_cast<int>(raw::Key::R)},
		{"f", static_cast<int>(raw::Key::F)},
		{"w", static_cast<int>(raw::Key::W)},
		{"a", static_cast<int>(raw::Key::A)},
		{"s", static_cast<int>(raw::Key::S)},
		{"d", static_cast<int>(raw::Key::D)},
		{"mouse_right", static_cast<int>(raw::Mouse::BUTTON_RIGHT)},
		{"mouse_left",	static_cast<int>(raw::Mouse::BUTTON_LEFT)},
		{"left_shift", 	static_cast<int>(raw::Key::LEFT_SHIFT)},
		{"left_ctrl", 	static_cast<int>(raw::Key::LEFT_CONTROL)},
		{"mouse_x", static_cast<int>(raw::Axes::MOUSE_X)},
		{"mouse_y", static_cast<int>(raw::Axes::MOUSE_Y)},
		{"mouse_scroll_x", static_cast<int>(raw::Axes::MOUSE_SCROLL_X)},
		{"mouse_scroll_y", static_cast<int>(raw::Axes::MOUSE_SCROLL_Y)},
	};
	static std::unordered_map<std::string, int> input_type_map = {
		{"render_wireframe",	static_cast<int>(InputAction::RENDER_WIREFRAME)},
		{"render_solid", 			static_cast<int>(InputAction::RENDER_SOLID)},
		{"move_forward", 			static_cast<int>(InputState::MOVE_FORWARD)},
		{"move_left", 				static_cast<int>(InputState::MOVE_LEFT)},
		{"move_back", 				static_cast<int>(InputState::MOVE_BACK)},
		{"move_right", 				static_cast<int>(InputState::MOVE_RIGHT)},
		{"spawn_ray", 				static_cast<int>(InputState::SPAWN_RAY)},
		{"move_up",						static_cast<int>(InputState::MOVE_UP)},
		{"move_down", 				static_cast<int>(InputState::MOVE_DOWN)},
		{"cursor_down", 			static_cast<int>(InputState::CURSOR_DOWN)},
		{"range_cursor_x", 		static_cast<int>(InputRange::CURSOR_X)},
		{"range_cursor_y", 		static_cast<int>(InputRange::CURSOR_Y)},
		{"range_cursor_scroll_x", static_cast<int>(InputRange::CURSOR_SCROLL_X)},
		{"range_cursor_scroll_y", static_cast<int>(InputRange::CURSOR_SCROLL_Y)},
	};

	static std::unordered_map<int, KeyInfo> context_map;

	static bool is_range(int key){
		if(auto it = context_map.find(key);
				it != context_map.end()){
			return it->second.type == RANGE;
		}
	
		return false;
	}
	static bool is_action(int key){
		if(auto it = context_map.find(key);
				it != context_map.end()){
			return it->second.type == ACTION;
		}
	
		return false;
	}
	static bool is_state(int key){
		if(auto it = context_map.find(key);
				it != context_map.end()){
			return it->second.type == STATE;
		}
	
		return false;
	}



  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)window;
    (void)scancode;
    (void)mods;
    (void)action;

		if(is_action(key) && action == GLFW_PRESS){
			_event.actions.push_back(context_map[key].key);
		}else if(is_state(key)){
			if(action == GLFW_PRESS){
				_event.states.push_back(context_map[key].key);
			}else if (action == GLFW_RELEASE){
				auto it = std::find(
						_event.states.begin(),
						_event.states.end(),
						context_map[key].key);

				if(it != _event.states.end()){
					_event.states.erase(it);
				}
			}
		}

    //_ial->register_key(key, action, mods);
    // Connect to ImGui
    //ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
  }

  static void mouse_callback(GLFWwindow* window, double x, double y){
    (void)window;
    (void)x;
    (void)y;
		static double last_x = 0.0;
		static double last_y = 0.0;

		if(is_range(raw::Axes::MOUSE_X)){
			double normalized_x 	= 2.0*x/window_width - 1.0; // also flips axis
			double normalized_dx 	= normalized_x - last_x;
			last_x = normalized_x;

			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_X));
			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_DX));
			_event.range_values[static_cast<int>(InputRange::CURSOR_X)]  = normalized_x;
			_event.range_values[static_cast<int>(InputRange::CURSOR_DX)] = normalized_dx;
		}
		
		if(is_range(raw::Axes::MOUSE_Y)){
			double normalized_y = 1.0 - 2.0f*y/window_height; // also flips axis
			double normalized_dy = normalized_y - last_y;
			last_y = normalized_y;

			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_Y));
			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_DY));
			_event.range_values[static_cast<int>(InputRange::CURSOR_Y)]  = normalized_y;
			_event.range_values[static_cast<int>(InputRange::CURSOR_DY)] = normalized_dy;
		}
		//_ial->register_mouse(x, y, window_width, window_height);
  }

  static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    // Connect to ImGui
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);

		if(is_action(button) && action == GLFW_PRESS){
			_event.actions.push_back(context_map[button].key);
		}else if(is_state(button)){
			if(action == GLFW_PRESS){
				_event.states.push_back(context_map[button].key);
			}else if (action == GLFW_RELEASE){
				auto it = std::find(
						_event.states.begin(),
						_event.states.end(),
						context_map[button].key);

				if(it != _event.states.end()){
					_event.states.erase(it);
				}
			}
		}
    //_ial->register_key(button, action, mods);
  }

  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    // Connect to ImGui
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);

		// TODO scroll input is not normalized (undefined range)
		if(is_range(raw::Axes::MOUSE_SCROLL_X)){
			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_SCROLL_X));
			_event.range_values[static_cast<int>(InputRange::CURSOR_SCROLL_X)] = xoffset;
		}

		if(is_range(raw::Axes::MOUSE_SCROLL_Y)){
			_event.ranges.push_back(static_cast<int>(InputRange::CURSOR_SCROLL_Y));
			_event.range_values[static_cast<int>(InputRange::CURSOR_SCROLL_Y)] = yoffset;
		}

  }

  static void character_callback(GLFWwindow* window, unsigned int codepoint){
    (void)window;
    (void)codepoint;
    // Connect to ImGui
    ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
  }

  static void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods){
    (void)window;
    (void)codepoint;
    (void)mods;
  }

	

	static bool starts_with(const std::string_view& view, const std::string& value){
		return view.substr(0, value.size()).compare(value) == 0;
	}

	static void process_line(std::string_view line_view, 
			const std::string& file, 
			unsigned int line_number){

		// Trim leading and trailing whitespace
		size_t pos;
		if((pos = line_view.find_first_not_of(' ')) != std::string_view::npos){
			line_view.remove_prefix(pos);
		}
		if((pos = line_view.find_last_not_of(' ')) != std::string_view::npos){
			line_view.remove_suffix(line_view.size() - pos - 1);
		}

		// if the line is empty after the whitespace trim, return
		if(line_view.size() == 0){
			return;
		}

		if(starts_with(line_view, "#")){
			// Comment
		}else if(starts_with(line_view, "--")){
			// context
		}else{
			// declaration
			// get type used
			KeyType key_type = ACTION; // default

			if(pos = line_view.find(' '); pos != std::string_view::npos){
				auto type_str = line_view.substr(0, pos);
				line_view.remove_prefix(pos + 1);
				if(type_str == "action"){
					key_type = ACTION;
				}else if(type_str == "state"){
					key_type = STATE;
				}else if(type_str == "range"){
					key_type = RANGE;
				}else{
					spdlog::get("console")->error("Error in Input-Config-File: {}: {}\t Unknown key type {}!", file, line_number, (std::string)line_view);
					return;
				}
			}else{
				spdlog::get("console")->error("Error in Input-Config-File: {}:{}\t Missing space after variable name in \"{}\"", file, line_number, (std::string)line_view);
				return;
			}

			// find key
			std::string key;
			if(pos = line_view.find(' '); pos != std::string_view::npos){
				key = line_view.substr(0, pos);
				line_view.remove_prefix(pos + 1); // +1 to include whitespace
			}else{
				spdlog::get("console")->error("Error in Input-Config-File: {}:{}\t Missing space after variable name in \"{}\"", file, line_number, (std::string)line_view);
				return;
			}

			// find and interpret variable

			// Trim whitespace
			if((pos = line_view.find_first_not_of(' ')) != std::string_view::npos){
				line_view.remove_prefix(pos);
			}

			auto it = key_map.find(key);
			if(it != key_map.end()){
				context_map[key_map[key]] = {input_type_map[(std::string)line_view], key_type};
				spdlog::get("console")->info("key: {} -> {}", key, (std::string)line_view);
			}
		}
	}

	static void read_input_config_file(const std::string& file){
		std::ifstream file_stream(file);

		if(file_stream.is_open()){
			std::string line_data;
			unsigned int line_number = 0;

			while(std::getline(file_stream, line_data)){
				line_number++;
				process_line(line_data, file, line_number);
			}
		}else{
			spdlog::get("console")->error("could not load file {}!", file);
		}
	}
}

void Input::configure(EventManager& events){
	read_input_config_file("../res/input.config");

  System<Input>::configure(events);
  (void)events;

  // Events
  events.subscribe<event::ActiveWindow>(*this);

  // set the context to the current one
  // @TODO easy way to switch between contexts
  _window_context = Window::current();

  // Could I loop through all the contexts and force them to this
  // callback?
  glfwSetKeyCallback(_window_context, key_callback);
  glfwSetCursorPosCallback(_window_context, mouse_callback);
  glfwSetMouseButtonCallback(_window_context, mouse_button_callback);
  glfwSetScrollCallback(_window_context, scroll_callback);
  glfwSetCharCallback(_window_context, character_callback);
  //glfwSetCharModsCallback(_window_context, charmods_callback);

  // Setup ImGui binding
  // TODO move?
  ImGui_ImplGlfwGL3_Init(_window_context, false);
}

void Input::update(EntityManager& entities,
                   EventManager& events,
                   f32 dt){
  (void) entities;
  (void) dt;

  glfwPollEvents();
  //events.emit(_ial->get_active());
	events.emit(_event);
	_event.actions.clear();
	_event.ranges.clear();
}

void Input::receive(event::ActiveWindow& event){
  window_width = event.window->width();
  window_height = event.window->height();
}

} // namespace weq::system
