#include <weq/vars/Vars.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <string_view>
#include <algorithm>







struct Audio {
	Var(float, mix_all, 1.0)
	Var(float, mix_ambiences, 1.0)
	Var(float, mix_footsteps, 1.0)
	Var(float, mix_props, 1.0)
	Var(float, mix_ui, 1.0)
	Var(float, mix_voice, 1.0)
	Var(float, mix_movies, 1.0)
  Var(bool, profiling, true)
  Var(std::string, title, "nope");
  Var(int, number, 15);
};

Audio audio;




namespace weq::vars{
using VarMap = std::unordered_map<std::string, std::tuple<void*, size_t, size_t>>;

VarMap& get_var_map(){
  static VarMap var_map;
  return var_map;
}

template<typename T>
bool is_hash_code(size_t hash_code){
  return hash_code == typeid(T).hash_code();
}

bool starts_with(const std::string_view& view, const std::string& value){
  return view.substr(0, value.size()).compare(value) == 0;
}

void process(std::string_view line_view, const std::string& filepath, unsigned int line_number){
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

  // Process the line
  if(starts_with(line_view, "#")){
    // Comment
  }else if(starts_with(line_view, "--")){
    // Directory
  }else{
    // Declaration

    // find variable name
    std::string var;
    if((pos = line_view.find(' ')) != std::string_view::npos){
      var = line_view.substr(0, pos);
      line_view.remove_prefix(pos + 1); // +1 to include whitespace
    }else{
      spdlog::get("console")->error("Error in Var-File: {}:{}\t Missing space after variable name in \"{}\"", filepath, line_number, (std::string)line_view);
      return;
    }

    // find and interpret value

    // Trim whitespace
    if((pos = line_view.find_first_not_of(' ')) != std::string_view::npos){
      line_view.remove_prefix(pos);
    }

    // Interpret value
    auto it = get_var_map().find(var);
    if(it != get_var_map().end()){
      auto [ptr, size, hash_code] = it->second;

      if(is_hash_code<int>(hash_code)){
        *((int*)ptr) = std::stoi((std::string)line_view);
      }else if(is_hash_code<float>(hash_code)){
        *((float*)ptr) = std::stof((std::string)line_view);
      }else if(is_hash_code<bool>(hash_code)){
        *((bool*)ptr) = (line_view.compare("true") == 0);
      }else if(is_hash_code<std::string>(hash_code)){
        *((std::string*)ptr) = (std::string)line_view;
      }else{
        spdlog::get("console")->error("Error when interpreting Var-File: {}:{}\t Unsupported cpp type declaration for {}", filepath, line_number, var);
      }
    }else{
      spdlog::get("console")->error("Error when intpreting Var-File: {}:{}\t Missing cpp declaration for {}", filepath, line_number, var);
    }
  }
}

void initialize(const std::string& filepath){
  std::ifstream file_stream(filepath);

  if(file_stream.is_open()){
    std::string line_data;
    unsigned int line_number = 0;

    while(std::getline(file_stream, line_data)){
      line_number++;
      process(line_data, filepath, line_number);
    }
  }else{
    spdlog::get("console")->error("Could not load file {}!", filepath);
  }
}

void attach(const char* name, void* pointer, size_t size, size_t hash_code){
	get_var_map().insert({ name, std::make_tuple(pointer, size, hash_code) });
};

}
