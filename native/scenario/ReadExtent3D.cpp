//
// Created by Ingun Jon on 1/14/26.
//

#include "ReadExtent3D.h"

AppPtr ReadExtent3D::run(std::string input) {
    return next(my_io::parse_Extent3D(input));
}