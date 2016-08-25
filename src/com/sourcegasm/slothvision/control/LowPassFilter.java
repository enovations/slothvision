package com.sourcegasm.slothvision.control;

public class LowPassFilter {
	private boolean first = true;
	public double smoothedValue;
	public double smoothing;

	public LowPassFilter(double smoothing) {
		this.smoothing = smoothing;
	}

	public double calculate(double newValue) {
		if (first) {
			smoothedValue = newValue;
			first = false;
			return newValue;
		} else {
			smoothedValue = ((smoothing - 1) * smoothedValue + newValue) / smoothing;
			return smoothedValue;
		}
	}

	public void clear() {
		first = true;
	}
}
