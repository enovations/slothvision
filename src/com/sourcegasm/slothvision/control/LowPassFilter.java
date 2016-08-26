package com.sourcegasm.slothvision.control;

class LowPassFilter {
	private boolean first = true;
	private double smoothedValue;
	private double smoothing;

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
