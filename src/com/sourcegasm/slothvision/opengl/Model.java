package com.sourcegasm.slothvision.opengl;

import static org.lwjgl.opengl.GL11.GL_FLOAT;
import static org.lwjgl.opengl.GL15.*;
import static org.lwjgl.opengl.GL20.glVertexAttribPointer;
import static org.lwjgl.opengl.GL30.glBindVertexArray;
import static org.lwjgl.opengl.GL30.glDeleteVertexArrays;
import static org.lwjgl.opengl.GL30.glGenVertexArrays;

import java.awt.image.BufferedImage;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;

public class Model {

	public int vao;
	public int textureID;
	public int size;

	public int programID;

	private final BufferedImage texture;
	private final ArrayList<Integer> buffers = new ArrayList<Integer>();

	public Model(float[] vertices, int[] indices, float[] textureCoords, BufferedImage texture, String vertexShaderLoc,
			String fragmentShaderLoc) {
		vao = glGenVertexArrays();
		glBindVertexArray(vao);
		storeIndices(indices);
		storeFloatInBuffer(vertices, 0, 3);
		storeFloatInBuffer(textureCoords, 1, 2);
		glBindVertexArray(0);

		this.texture = texture;
		textureID = TextureLoader.loadTexture(this.texture);

		programID = ShaderUtils.load(vertexShaderLoc, fragmentShaderLoc);

		size = indices.length;
	}

	public void delete() {
		for (final int vbo : buffers)
			glDeleteBuffers(vbo);

		glDeleteVertexArrays(vao);
	}

	private void storeFloatInBuffer(float[] data, int index, int size) {
		final int vbo = glGenBuffers();
		buffers.add(vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		final FloatBuffer buffer = Utilities.createFloatBuffer(data);
		glBufferData(GL_ARRAY_BUFFER, buffer, GL_STATIC_DRAW);
		glVertexAttribPointer(index, size, GL_FLOAT, false, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	private void storeIndices(int[] indices) {
		final int vbo = glGenBuffers();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
		final IntBuffer buffer = Utilities.createIntBuffer(indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer, GL_STATIC_DRAW);
	}
}