package com.sourcegasm.slothvision.control;

public class PID {
	public static interface Direction {
		public static int NORMAL = 1;
		public static int REVERSED = -1;
	}

	// Some variables
	float p, i, d;
	float err;
	long prevTime = -1;
	float prevErr;
	float integration;
	int direction = 1;

	float imax;

	public PID(float p, float i, float d, float imax, int direction) {
		this.p = p;
		this.i = i;
		this.d = d;
		this.imax = imax;
		this.direction = direction;
	}

	public float calculate(float input, float setpoint) {

		if (prevTime != -1) {

			float dt = (System.nanoTime() - prevTime) / 1000000000.0f;
			prevTime = System.nanoTime();

			float err = setpoint - input;
			float diff = (err - prevErr) / dt;
			integration += err * dt;

			prevErr = err;

			if (integration > imax)
				integration = imax;
			if (integration < -imax)
				integration = -imax;

			return (err * p + integration * i + diff * d) * direction;

		} else {
			prevTime = System.nanoTime();
			return 0;
		}

	}

	public void resetPID() {
		prevErr = 0;
		integration = 0;
		prevTime = -1;
	}
}
