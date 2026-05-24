#pragma once
#include "graphics/mesh.h"

using namespace std;
using namespace gpu;

void Mesh::setVerticies(const vector<Vertex>& verts, const vector<GLuint>& indicies)
{
    this->vertices = verts;
    this->indicies = indicies;

    vertexCount = verts.size();
    indexCount = indicies.size();
}