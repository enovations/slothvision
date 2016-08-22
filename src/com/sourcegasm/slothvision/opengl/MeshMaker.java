package com.sourcegasm.slothvision.opengl;

import java.util.ArrayList;

public class MeshMaker {
	private static final int SIZE = 100;

	public static float[] leftMesh() {
		final ArrayList<Float> vertices = new ArrayList<Float>();

		for (float y = 1; y >= -1; y -= 1f / SIZE) {
			for (float x = -1; x <= 0; x += 1f / SIZE) {
				vertices.add(x);
				vertices.add(y);
				vertices.add(1f);
			}
		}

		final float[] result = new float[vertices.size()];

		for (int i = 0; i < result.length; i++)
			result[i] = vertices.get(i);

		return result;
	}

	public static float[] rightMesh() {
		final ArrayList<Float> vertices = new ArrayList<Float>();

		for (float y = 1; y >= -1; y -= 1f / SIZE) {
			for (float x = 0; x <= 1; x += 1f / SIZE) {
				vertices.add(x);
				vertices.add(y);
				vertices.add(1f);
			}
		}

		final float[] result = new float[vertices.size()];

		for (int i = 0; i < result.length; i++)
			result[i] = vertices.get(i);

		return result;
	}

	public static float[] distortMesh(float[] mesh, boolean left) {
		final float calibration = -0.068731f;
		final float paramA = -0.007715f;
		final float paramC = 0f;
		final float paramD = 1f - paramA - calibration - paramC;

		final float centerY = 0f;
		float centerX;

		if (left)
			centerX = -0.5f;
		else
			centerX = 0.5f;

		final float d = 0.25f;

		for (int i = 0; i < mesh.length; i += 3) {
			final float deltaX = (mesh[i] - centerX) / d;
			final float deltaY = (mesh[++i] - centerY) / d;

			final float dstR = (float) Math.sqrt(deltaX * deltaX + deltaY * deltaY);
			final float srcR = (paramA * dstR * dstR * dstR + calibration * dstR * dstR + paramC * dstR + paramD)
					* dstR;

			final float factor = Math.abs(dstR / srcR);

			final float resultX = centerX + (deltaX * factor * d);
			final float resultY = centerY + (deltaY * factor * d);

			if (resultX < 0 && resultY < 1 && resultX > -1 && resultY > -1) {
				mesh[i] = resultX;
				mesh[++i] = resultY;
			}
		}

		return mesh;
	}

	public static float[] textureCoords() {
		final ArrayList<Float> coordinates = new ArrayList<Float>();

		for (float y = 0f; y <= 1.0f; y += 1f / SIZE / 2f) {
			for (float x = 0f; x <= 1.0f; x += 1f / SIZE) {
				coordinates.add(x);
				coordinates.add(y);
			}
		}

		final float[] result = new float[coordinates.size()];

		for (int i = 0; i < result.length; i++)
			result[i] = coordinates.get(i);

		return result;
	}

	public static int[] indices() {
		final ArrayList<Integer> indices = new ArrayList<Integer>();

		for (int y = 0; y < SIZE * 2; ++y) {
			for (int x = 0; x < SIZE; ++x) {

				indices.add(y * (SIZE + 1) + x);
				indices.add(y * (SIZE + 1) + SIZE + x + 1);
				indices.add(y * (SIZE + 1) + x + 1);
				indices.add(y * (SIZE + 1) + x + 1);
				indices.add(y * (SIZE + 1) + SIZE + x + 1);
				indices.add(y * (SIZE + 1) + SIZE + x + 2);
			}
		}

		final int[] result = new int[indices.size()];

		for (int i = 0; i < result.length; i++)
			result[i] = indices.get(i);

		return result;
	}
}