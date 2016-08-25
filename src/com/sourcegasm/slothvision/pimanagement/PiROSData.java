package com.sourcegasm.slothvision.pimanagement;

import java.util.concurrent.CopyOnWriteArrayList;

public class PiROSData {

	public double battery = 0.0;
	public CopyOnWriteArrayList<Point> points = new CopyOnWriteArrayList<>();
	public double yaw, x, y;
	public int mode = 0;
	public int marker_distance;
    public int marker_x;


    public static class Point {

		public double x, y;

		public Point(double x, double y) {
			this.x = x;
			this.y = y;
		}

	}

}
