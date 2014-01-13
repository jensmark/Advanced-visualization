//
//  Timer.hpp
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#ifndef GLAppNative_Timer_hpp
#define GLAppNative_Timer_hpp

#include "GLFW/glfw3.h"

class Timer{
public:
    Timer() : startTime(glfwGetTime()) {};
    
	/**
	 * Report the elapsed time in seconds (it will return a double,
	 * so the fractional part is subsecond part).
	 */
	inline double elapsed() const {
		return glfwGetTime() - startTime;
	};
    
	/**
	 * Report the elapsed time in seconds, and reset the timer.
	 */
	inline double elapsedAndRestart() {
		double now = glfwGetTime();
		double elapsed = now - startTime;
		startTime = now;
		return elapsed;
	};
    
	/**
	 * Restart the timer.
	 */
	inline void restart() {
		startTime = glfwGetTime();
	};
    
private:
    double startTime;
};

#endif