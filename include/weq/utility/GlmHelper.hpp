#pragma once

//#include <spdlog/spdlog.h>
//#include <glm/glm.hpp>
//#include <string>

namespace weq{
static void draw_mat(std::string name, glm::mat4 m){
  spdlog::get("console")->info(
    "{}\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n",
    name,
    m[0][0], m[1][0], m[2][0], m[3][0],
    m[0][1], m[1][1], m[2][1], m[3][1],
    m[0][2], m[1][2], m[2][2], m[3][2],
    m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

static void draw_quat(std::string name, glm::quat q){
  spdlog::get("console")->info("{}\t[{}, {}, {}, {}]",
                               name,
                               q.x,
                               q.y,
                               q.z,
                               q.w
    );
}
}
