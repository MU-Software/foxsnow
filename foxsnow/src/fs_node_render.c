#include "fs_node_render.h"


void computeNormalOfPlane(float* normal, const float* pvector1, const float* pvector2) {
	normal[0] = (pvector1[1] * pvector2[2]) - (pvector1[2] * pvector2[1]);
	normal[1] = (pvector1[2] * pvector2[0]) - (pvector1[0] * pvector2[2]);
	normal[2] = (pvector1[0] * pvector2[1]) - (pvector1[1] * pvector2[0]);
}
void glhTranslatef2(float* matrix, float x, float y, float z) {
	matrix[12] = matrix[0] * x + matrix[4] * y + matrix[8] * z + matrix[12];
	matrix[13] = matrix[1] * x + matrix[5] * y + matrix[9] * z + matrix[13];
	matrix[14] = matrix[2] * x + matrix[6] * y + matrix[10] * z + matrix[14];
	matrix[15] = matrix[3] * x + matrix[7] * y + matrix[11] * z + matrix[15];
}

void commitCamera() {

	float upVector3D[3] = { 0.0f, 1.0f, 0.0f };
	normalizeVector(upVector3D);
	float up[3] = { 0.0f, 0.0f, 0.0f };
	float size[3] = { 0.0f };
	float forward[3] = { 0.0f };
	forward[0] = camera.lookPoint[0] - camera.pos[0];
	forward[1] = camera.lookPoint[1] - camera.pos[1];
	forward[2] = camera.lookPoint[2] - camera.pos[2];
	normalizeVector(forward);
	computeNormalOfPlane(&size, &forward, &upVector3D);
	normalizeVector(size);
	computeNormalOfPlane(&up, &size, &forward);

	free_matrix(&FS_ViewMatrix);
	FS_ViewMatrix = create_identity_matrix(4, 4);
	//matrix* lookup_matrix = create_identity_matrix(4, 4);
	FS_ViewMatrix->mat[0] = size[0];
	FS_ViewMatrix->mat[4] = size[1];
	FS_ViewMatrix->mat[8] = size[2];
	FS_ViewMatrix->mat[1] = up[0];
	FS_ViewMatrix->mat[5] = up[1];
	FS_ViewMatrix->mat[9] = up[2];
	FS_ViewMatrix->mat[2] = -forward[0];
	FS_ViewMatrix->mat[6] = -forward[1];
	FS_ViewMatrix->mat[10] = -forward[2];
	glhTranslatef2(FS_ViewMatrix->mat, camera.pos[0], camera.pos[1], camera.pos[2]);

	//FS_ProjectionMatrix = mat_multiply(FS_ProjectionMatrix_withoutCamera, lookup_matrix);
	//free_matrix(&lookup_matrix);
}

void renderNodeIn(node* target) {
	if (!target) return;
	fs_3d_data* target_data = (fs_3d_data*)(target->data);
	apply_data(target_data);
	if (target->parent) {
		if (target_data->cumulative_model_mat)
			free_matrix(&(target_data->cumulative_model_mat));
		if (target_data->cumulative_model_mat_without_scale)
			free_matrix(&(target_data->cumulative_model_mat_without_scale));

		target_data->cumulative_model_mat = mat_square_multiply(
			target_data->model_mat,
			((fs_3d_data*)(target->parent->data))->cumulative_model_mat_without_scale);
		target_data->cumulative_model_mat_without_scale = mat_square_multiply(
			target_data->model_mat_without_scale,
			((fs_3d_data*)(target->parent->data))->cumulative_model_mat_without_scale);
	}
	else {
		if (!(target_data->cumulative_model_mat))
			target_data->cumulative_model_mat = create_matrix(4, 4,
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		if (!(target_data->cumulative_model_mat_without_scale))
			target_data->cumulative_model_mat_without_scale = create_matrix(4, 4,
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		memcpy(target_data->cumulative_model_mat->mat,
			target_data->model_mat->mat,
			sizeof(float) * 4 * 4);
		memcpy(target_data->cumulative_model_mat_without_scale->mat,
			target_data->model_mat_without_scale->mat,
			sizeof(float) * 4 * 4);
	}
}

void renderNodeOut(node* target) {
	fs_3d_data* target_data = (fs_3d_data*)(target->data);
	if (target_data->vertex_array) {
		glBindVertexArray(target_data->vertex_array);
		glUseProgram(target_data->shader_program);

		GLint uniform_model_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ModelMatrix");
		GLint uniform_view_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ViewMatrix");
		GLint uniform_proj_mat_loc = glGetUniformLocation(target_data->shader_program, "fs_ProjectionMatrix");
		glUniformMatrix4fv(uniform_model_mat_loc, 1, GL_FALSE, target_data->cumulative_model_mat->mat);
		glUniformMatrix4fv(uniform_view_mat_loc, 1, GL_FALSE, FS_ViewMatrix->mat);
		glUniformMatrix4fv(uniform_proj_mat_loc, 1, GL_FALSE, FS_ProjectionMatrix->mat);

		GLint uniform_frame_number_loc = glGetUniformLocation(target_data->shader_program, "fs_FrameNumber");
		GLint uniform_screen_size_loc = glGetUniformLocation(target_data->shader_program, "fs_ScreenSize");
		glUniform1ui(uniform_frame_number_loc, frame_number);
		glUniform2ui(uniform_screen_size_loc, current_resolution_x, current_resolution_y);

		GLint uniform_object_position_loc = glGetUniformLocation(target_data->shader_program, "fs_ObjectPosition");
		glUniform3fv(uniform_object_position_loc, 3, target_data->pos);

		GLint uniform_camera_position_loc = glGetUniformLocation(target_data->shader_program, "fs_CameraPosition");
		glUniform3fv(uniform_camera_position_loc, 3, camera.pos);

		int current_texture_index = 0;
		list_element* target_texture_element = target_data->texture_list->head;
		while (target_texture_element) {
			glActiveTexture(GL_TEXTURE0 + current_texture_index);
			glBindTexture(GL_TEXTURE_2D, ((TextureInfo*)(target_texture_element->data))->texture_id);

			char texture_uniform_name[32] = { 0 };
			sprintf(texture_uniform_name, "fs_Texture%d", current_texture_index);
			int texture_uniform_loc = glGetUniformLocation(target_data->shader_program, texture_uniform_name);
			if (texture_uniform_loc != -1) {
				glUniform1i(texture_uniform_loc, current_texture_index);
			}
			target_texture_element = target_texture_element->next;
		}

		if (target_data->do_backface_cull) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);

		if (target_data->element_buffer) {
			glDrawElements(GL_TRIANGLES, target_data->element_buffer_size, GL_UNSIGNED_INT, 0);
		}
	}
}

int renderNode(node* target, int level) {
	if (!target) return;

	target->in(target);
	node* target_child = target->child;
	while (target_child) {
		renderNode(target_child, level + 1);
		target_child = target_child->next;
	}
	target->out(target);
	return 0;
}