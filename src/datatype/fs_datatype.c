#include "fs_datatype.h"

void renderNodeIn(node* target) {
	if (!target) return;
	apply_data(target->data);

	if(target->parent) {
		if (!(target->parent->data->cumulative_model_mat)) {
			target->parent->data->cumulative_model_mat = create_matrix(4, 4,
															0.0f, 0.0f, 0.0f, 0.0f,
															0.0f, 0.0f, 0.0f, 0.0f,
															0.0f, 0.0f, 0.0f, 0.0f,
															0.0f, 0.0f, 0.0f, 0.0f);
			memcpy(target->parent->data->cumulative_model_mat->mat,
				   target->parent->data->model_mat->mat,
				   sizeof(float)*4*4);
		}
		target->data->cumulative_model_mat = mat_square_multiply(
												target->data->model_mat,
												target->parent->data->cumulative_model_mat);
	}
	else {
		target->data->cumulative_model_mat = create_matrix(4, 4,
												0.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f, 0.0f);
		memcpy(target->data->cumulative_model_mat->mat,
			   target->data->model_mat->mat,
			   sizeof(float)*4*4);
	}
}

void renderNodeOut(node* target) {
	if (target->data->model_vertex_array) {
		glBindVertexArray(target->data->vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, target->data->vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, target->data->element_buffer);
		glUseProgram(target->data->program);
		
		GLint uniform_model_mat_loc = glGetUniformLocation(target->data->program, "fs_ModelMatrix");
		GLint uniform_view_mat_loc  = glGetUniformLocation(target->data->program, "fs_ViewMatrix");
		GLint uniform_proj_mat_loc  = glGetUniformLocation(target->data->program, "fs_ProjectionMatrix");
		glUniformMatrix4fv(uniform_model_mat_loc, 1, GL_FALSE, target->data->cumulative_model_mat->mat);
		glUniformMatrix4fv(uniform_view_mat_loc, 1, GL_FALSE, FS_ViewMatrix->mat);
		glUniformMatrix4fv(uniform_proj_mat_loc, 1, GL_FALSE, FS_ProjectionMatrix->mat);
		if (target->data->element_buffer) {
			glDrawElements(GL_TRIANGLES, target->data->model_index_size, GL_UNSIGNED_INT, 0);
		}
	}
}

int renderNode(node* target, int level) {
	if (!target) return;

	target->in(target);
	node* target_child = target->child;
	while(target_child) {
		node_postorder(target_child, level+1);
		target_child = target_child->next;
	}
	target->out(target);
}