#include "Mesh.h"

// OpenGL / GLEW
#include "glew.h"
//#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
Mesh::Mesh()
{
    //SetUpMesh();
}

Mesh::~Mesh()
{
}




void Mesh::SetUpMesh()
{
	SetUpTexture();
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);

	// Vertex Array Object (VAO) --------------------------------------------------
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertices
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	// Normals
	glGenBuffers(1, &id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(1);


	glGenBuffers(1, &id_tex_coord);
	glBindTexture(GL_TEXTURE_BUFFER, id_tex_coord);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * num_tex_coords * 2, tex_coords, GL_STATIC_DRAW);

	// Add texture position attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// Unbind any vertex array we have binded before.
	glBindVertexArray(0);

}

void Mesh::SetUpTexture()
{
    GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
        0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	glBindTexture(GL_TEXTURE_2D, textureId);
	// draw mesh
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glDrawElements(GL_TRIANGLES,num_indices, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
