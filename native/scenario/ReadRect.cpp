//
// Created by Ingun Jon on 1/14/26.
//

#include "ReadRect.h"

#include <my_io.h>

AppPtr ReadRect::run(std::string input) {
    return next(my_io::parse_rect(input));
}