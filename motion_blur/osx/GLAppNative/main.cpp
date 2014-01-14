//
//  main.cpp
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#include "AppManager.h"

int main(int argc, const char * argv[])
{
    AppManager* manager = NULL;
    try {
        manager = new AppManager();
        manager->init();
        manager->begin();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    delete manager;
    return 0;
}

