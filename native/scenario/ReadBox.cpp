//
// Created by Ingun Jon on 1/14/26.
//

#include "ReadBox.h"

AppPtr ReadBox::run(std::string input) {
    return next(my_io::parse_box(input));
}