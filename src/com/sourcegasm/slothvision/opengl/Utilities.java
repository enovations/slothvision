package com.sourcegasm.slothvision.opengl;

import java.io.BufferedReader;
import java.io.FileReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import org.lwjgl.BufferUtils;

public class Utilities {

	public static String loadAsString(String location) {
		final StringBuilder result = new StringBuilder();
		try {
			final BufferedReader reader = new BufferedReader(new FileReader(location));
			String buffer = "";
			while ((buffer = reader.readLine()) != null) {
				result.append(buffer).append("\n");
			}
			reader.close();
		} catch (final Exception e) {
			System.err.println(e);
		}

		return result.toString();
	}

	public static ByteBuffer createByteBuffer(byte[] array) {
		final ByteBuffer result = ByteBuffer.allocateDirect(array.length).order(ByteOrder.nativeOrder());
		result.put(array).flip();

		return result;
	}

	public static FloatBuffer createFloatBuffer(float[] array) {
		final FloatBuffer result = ByteBuffer.allocateDirect(array.length << 2).order(ByteOrder.nativeOrder())
				.asFloatBuffer();
		result.put(array).flip();

		return result;
	}

	public static IntBuffer createIntBuffer(int[] array) {
		final IntBuffer buffer = BufferUtils.createIntBuffer(array.length);
		buffer.put(array);
		buffer.flip();

		return buffer;
	}
}