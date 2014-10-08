#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/freeglut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <vector>

#define GLM_FORCE_RADIANS // added to remove warning messages about depreciaton
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier


#include <assimp/Importer.hpp> //includes the importer, which is used to read our obj file
#include <assimp/scene.h> //includes the aiScene object
#include <assimp/postprocess.h> //includes the postprocessing variables for the importer
#include <assimp/color4.h> //includes the aiColor4 object, which is used to handle the colors from the mesh objects

//#include <Magick++.h>
#include <Magick++/Blob.h>
#include <Magick++/Image.h>

#include "shaderLoader.h"

//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat uv[2];
};

//--Evil Global variables
//Just for this example
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint gSampler;// sampler
GLuint vbo_geometry;// VBO handle for our geometry

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_color;

//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection

std::vector<Vertex> geometry;

//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);

//--Resource management
bool initialize(char*,char*);
void cleanUp();

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

// obj model loader
bool loadObjModel(char* obj, char* tex, std::vector<Vertex> & out_vertices);
void Bind();

//---------------
GLenum m_textureTarget;
GLuint m_textureObj;
Magick::Image* m_pImage;
Magick::Blob m_blob;
//---------------

//--Main
int main(int argc, char **argv)
{
    char* fName = new char[256];
    char* texName = new char[256];

    // get fileName from command line
    if(argc > 2)
        {
         strcpy(fName, argv[1]);
         strcpy(texName, argv[2]);
        }
    else
        {
         std::cout<<"[F] FAILED TO ADD OBJ FILE OR TECTURE AS ARGUMENT"<<std::endl;
         return -1;
        }
 
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("Matrix Example");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize(fName, texName);
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);

    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,uv));

    //TO-DO glBindTexture();
    Bind();

    glDrawArrays(GL_TRIANGLES, 0, geometry.size());//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
                           
    //swap the buffers
    glutSwapBuffers();
}

void update()
{

    //total time
    static float angle = 0.0;
    static float rotateAngle = 0.0;
    float dt = getDT();// if you have anything moving, use dt.

    angle += dt * M_PI/6; //move through 90 degrees a second
    rotateAngle += dt * M_PI/6; // move the opposite direction at 45 degrees/second
/*
    // make cube orbit
    model = glm::translate( glm::mat4(1.0f), glm::vec3(5.0 * sin(angle), 0.0, 5.0 * cos(angle)));
*/
    // rotate from origin
    model = glm::rotate(glm::mat4(1.0f), rotateAngle, glm::vec3(0.0,1.0,0.0));


    //model = glm::translate( glm::mat4(1.0f), glm::vec3(1,0,0));

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        glutLeaveMainLoop();
    }
}

bool initialize(char* fName, char* texName)
{
    bool fileLoaded = loadObjModel(fName, texName, geometry);

    // attempt to load model
    if(!fileLoaded)
        {
         std::cerr << "[F] FAILED TO LOAD OBJ FILE!" << std::endl;
         return false;
        }

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, geometry.size() * sizeof(Vertex), &geometry[0], GL_STATIC_DRAW);

    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // get shader data from the bin
    shaderLoader* vShader = new shaderLoader("VertexShader", "../bin/vertex.shader", false);
    shaderLoader* fShader = new shaderLoader("fragmentShader", "../bin/fragment.shader", true);
    const char* vs = vShader->getShaderData();
    const char* fs = fShader->getShaderData();

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    char buffer[512];
    if(!shader_status)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, buffer);
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        std::cerr << buffer << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, buffer);
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        std::cerr << buffer << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    // add sampler to program
    gSampler = glGetUniformLocation(program, "gSampler");
    glUniform1i(gSampler, 0);

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_texture"));
    if(loc_color == -1)
    {
        std::cerr << "[F] V_TEXTURE NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry);
}

//returns the time delta
float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

bool loadObjModel(char* obj, char* tex, std::vector<Vertex> & out_vertices)
{
 std::vector<unsigned int> vertexIndices;
 std::vector<Vertex> temp_vertices;

 aiString path;
 //Magick::Image* m_pImage;
 //Magick::Blob m_blob;
 Assimp::Importer Importer;

 const aiScene* pScene = Importer.ReadFile(obj, aiProcess_Triangulate 
                                            | aiProcess_GenSmoothNormals 
                                            | aiProcess_FlipUVs);

 aiMesh* mesh = pScene->mMeshes[0];
 aiMaterial* mtl = pScene->mMaterials[0];
 mtl->GetTexture(aiTextureType_DIFFUSE, 1, &path);
 //std::cout<<"path: "<<path.data<<std::endl;
 //std::cout<<pScene->mNumMeshes<<" "<<pScene->mNumMaterials<<std::endl;
 if(!pScene)
    {
     std::cout<<"Error parsing "<<obj<<std::endl;
     return false;
    }

 try 
 {
   m_pImage = new Magick::Image(tex);
   m_pImage->write(&m_blob, "RGBA");
 }
 catch (Magick::Error& Error) 
 {
   std::cout << "Error loading texture '" << tex << "': " << Error.what() << std::endl;
   return false;
 }

//-----------------------------------------------------------------------
 glGenTextures(1, &m_textureObj);
 glBindTexture(m_textureTarget, m_textureObj);
 glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
 glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
 glBindTexture(m_textureTarget, 0);
//-----------------------------------------------------------------------


 //std::cout<<"rows: "<<m_pImage->rows()<<std::endl;
 for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
     const aiFace& face = mesh->mFaces[i];

     aiVector3D tempPos1 = mesh->mVertices[face.mIndices[0]];
     aiVector3D tempPos2 = mesh->mVertices[face.mIndices[1]];
     aiVector3D tempPos3 = mesh->mVertices[face.mIndices[2]];

     aiVector3D tempUV1 = mesh->mTextureCoords[0][face.mIndices[0]];
     aiVector3D tempUV2 = mesh->mTextureCoords[0][face.mIndices[1]];
     aiVector3D tempUV3 = mesh->mTextureCoords[0][face.mIndices[2]];

     Vertex tempVertex;

     // first point of triange
     tempVertex.position[0] = tempPos1.x;
     tempVertex.position[1] = tempPos1.y;
     tempVertex.position[2] = tempPos1.z;

     tempVertex.uv[0] = tempUV1.x;
     tempVertex.uv[1] = tempUV1.y;

     out_vertices.push_back(tempVertex);

     // second point of triange
     tempVertex.position[0] = tempPos2.x;
     tempVertex.position[1] = tempPos2.y;
     tempVertex.position[2] = tempPos2.z;

     tempVertex.uv[0] = tempUV2.x;
     tempVertex.uv[1] = tempUV2.y;

     out_vertices.push_back(tempVertex);

     // third point on triange
     tempVertex.position[0] = tempPos3.x;
     tempVertex.position[1] = tempPos3.y;
     tempVertex.position[2] = tempPos3.z;

     tempVertex.uv[0] = tempUV3.x;
     tempVertex.uv[1] = tempUV3.y;

     out_vertices.push_back(tempVertex);
    }

 return true;
}

void Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(m_textureTarget, m_textureObj);
}










