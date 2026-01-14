//
// Created by Ingun Jon on 1/14/26.
//

#include "ReadOrigin3D.h"

AppPtr ReadOrigin3D::run(std::string input) {
    return next(my_io::parse_Origin3D(input));
}