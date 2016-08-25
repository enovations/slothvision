package com.sourcegasm.slothvision.oculus;

import java.util.regex.Pattern;

public class Quaternion {
	public double q0;
	public double q1;
	public double q2;
	public double q3;

	public Quaternion() {
	}

	public Quaternion(String csvLine) {
		final String[] qs = csvLine.split(Pattern.quote(";"));
		q0 = Double.parseDouble(qs[0]);
		q1 = -Double.parseDouble(qs[3]);
		q2 = -Double.parseDouble(qs[1]);
		q3 = Double.parseDouble(qs[2]);
	}

	public Euler toEuler() {
		final Euler result = new Euler();
		result.roll =
				Math.atan2(2 * (q1 * q0 + q2 * q3), (q0 * q0) - (q1 * q1) - (q2 * q2) + (q3 * q3)) * 180.0 / Math.PI;
		result.pitch = -Math.asin(2 * (q1 * q3 - q2 * q0)) * 180.0 / Math.PI;
		result.yaw =
				Math.atan2(2 * (q3 * q0 + q1 * q2), (q0 * q0) + (q1 * q1) - (q2 * q2) - (q3 * q3)) * 180.0 / Math.PI;
		return result;
	}
}
