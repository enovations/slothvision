package com.sourcegasm.slothvision.control;

public class Limiter {

	public static double limitValues(double source, double absLimit) {

		if (source > absLimit)
			return absLimit;

		if (source < -absLimit)
			return -absLimit;

		return source;

	}

}
