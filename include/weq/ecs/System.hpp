#pragma once

#include <weq/utility/NumberTypes.hpp>

//#include <chrono> // needed for multiple dts
//using namespace std::chrono_literals;
//using std::chrono::nanoseconds;
//using std::chrono::duration_cast;
//using std::chrono::duration;
//
//// @TODO Temp?
//#include <spdlog/spdlog.h>


namespace weq{

class EventManager;
class EntityManager;

class BaseSystem{
public:
  virtual ~BaseSystem(){}

  virtual void configure(EventManager&){
    if(!_debug_name.empty()){
      spdlog::get("console")->info("configuring system {}", _debug_name);
    }
  }
  virtual void update(EntityManager&,EventManager&, f32) = 0;

  static u64 _type_counter;

  // extending for multiple dts
  const nanoseconds& get_timestep(){
    return _timestep;
  }

  double get_timestep_value(){
    return _timestep_value;
  }

  const nanoseconds& get_lag(){
    return _lag;
  }

  void set_lag(const nanoseconds& lag){
    _lag = lag;
  }

  void update_lag(){
	  //_lag = _lag - duration<long long, std::nano>((long long)_factor*_timestep.count());
    _lag = _lag - _timestep;
  }

  void increment_frame_counter(){_frame_counter++;}
  void clear_frame_counter(){_frame_counter = 0;}
  double get_framerate(){return _current_framerate;}
  void second_has_past(int divisor){_current_framerate = _frame_counter/divisor; clear_frame_counter();}
  unsigned int get_frame_counter(){return _frame_counter;}

  void set_debug_name(const std::string& str){_debug_name = str;}
  const std::string& get_debug_name(){return _debug_name;}

  double get_desired_framerate(){return 1.0/duration_cast<duration<double>>(_timestep).count();}
  void set_timestep(nanoseconds timestep){
    _timestep = timestep;
    _timestep_value = duration_cast<duration<double>>(_timestep).count();
  }

  bool active(){return _active;}
  void set_active(bool b){_active = b;}
  double get_factor(){return _factor;}
  void set_factor(double d){_factor = d;}

protected:

private:
  // Extending to support multiple dts
  //nanoseconds   _timestep{33333333ns};
  nanoseconds _timestep{16666667ns};
  //nanoseconds _timestep{1000000ns};
  nanoseconds _lag{0ns};
  double _timestep_value{0.01666667};
  //double _timestep_value{0.03333333};
  //double _timestep_value{0.001};
  double _current_framerate = 0.0;
  unsigned int _frame_counter{0};
  std::string _debug_name;
  bool _active = true;
  double _factor = 1.0f;
};

template<typename S>
class System : public BaseSystem{
public:
  virtual ~System(){}
private:
  friend class SystemManager;

  static u64 id(){
    static u64 id = _type_counter++;
    return id;
  }
};

}
