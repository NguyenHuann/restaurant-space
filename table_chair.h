#ifndef table_chair_h
#define table_chair_h

#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>


class Table_Chair {

public:
	std::vector<glm::mat4> modelMatrices;
	float tox, toy, toz;
	Table_Chair(float x = 0, float y = 0, float z = 0) {
		tox = x;
		toy = y;
		toz = z;
	}
	glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
		tx += tox;
		ty += toy;
		tz += toz;
		glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
		translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
		rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
		rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
		rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
		scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
		model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
		return model;
	}

	/*Shader local_rotation(Shader ourShader, unsigned int VAO, unsigned int VAO2, unsigned int VAO3, unsigned int VAO4, unsigned int VAO5, float angle = 0) {
		glm::mat4 model;
		float rotateAngle_X = 0;
		float rotateAngle_Y = 0;
		float rotateAngle_Z = 0;
		model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, 0.2, 1.75);
		modelMatrices.push_back(model);
		model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		model = transforamtion(1.15, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		model = transforamtion(1.15, 0, .75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		model = transforamtion(0, 0, .75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		model = transforamtion(0.4, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 0.1, 1);
		modelMatrices.push_back(model);
		model = transforamtion(0.4, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(.85, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(.85, -.35, 1.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(0.4, -.35, 1.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(0.75, -.3, 1.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, .3, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(0.525, -.3, 1.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, .3, 0.1);
		modelMatrices.push_back(model);
		model = transforamtion(0.475, .1, 1.175, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.8, -.6, 0.2);
		modelMatrices.push_back(model);
		unsigned int vertex_array[] = { VAO, VAO2, VAO2, VAO2, VAO2, VAO3, VAO3, VAO3, VAO3, VAO3, VAO4, VAO4, VAO5 };
		glm::vec3 averagePosition(0.0f);
		for (const glm::mat4& model : modelMatrices) {
			averagePosition += glm::vec3(model[3]); 
		}
		averagePosition /= modelMatrices.size();

		glm::mat4 moveToOrigin = glm::translate(glm::mat4(1.0f), -averagePosition);

		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 moveToOriginalPosition = glm::translate(glm::mat4(1.0f), averagePosition);

		glm::mat4 groupTransform = moveToOriginalPosition * rotation * moveToOrigin;

		int i = 0;
		for (glm::mat4& model : modelMatrices) {

			model = groupTransform * model;
			ourShader.setMat4("model", model);
			glBindVertexArray(vertex_array[i]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			i++;
		}
		return ourShader;
	}*/

	Shader ret_shader(Shader ourShader, unsigned int VAO, unsigned int VAO2, unsigned int VAO3, unsigned int VAO4, unsigned int VAO5) {
		glm::mat4 model;
		float rotateAngle_X = 0;
		float rotateAngle_Y = 0;
		float rotateAngle_Z = 0;
		//table surface
		model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, 0.2, 2.75);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//left top Leg
		model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//right top Leg
		model = transforamtion(2.15, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//right bottom Leg
		model = transforamtion(2.15, 0, 1.2650, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//left bottom Leg
		model = transforamtion(0, 0, 1.2650, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


		//near chair

		//chair_seat
		model = transforamtion(0, -.3, 1.52, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, -1.0, 1.3);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO3);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//c_Back
		model = transforamtion(0, 0.37, 2.07, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, -1.35, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO5);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



		//far chair

		//chair_seat
		model = transforamtion(0, -.3, -.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, -1.0, 1.3);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO3);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//c_Back
		model = transforamtion(0, 0.37, -.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.5, -1.35, 0.2);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO5);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//near seat holder
		model = transforamtion(0, -.8, -.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.01, 2.23, 0.13);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//far seat holder
		model = transforamtion(0, -.8, 2.109, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.01, 2.23, 0.13);
		modelMatrices.push_back(model);
		ourShader.setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		return ourShader;
	} 
};


#endif