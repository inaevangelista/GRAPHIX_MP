#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float mod_x = 0;
float y_mod = 0.0f;
float z_mod = -5.0f;
void Key_Callback(GLFWwindow* window,
    int key, //KeyCode
    int scanCode, //ScanCode
    int action, //Press / Release
    int mods //Modifier keys
)
{
    if (key == GLFW_KEY_D &&
        action == GLFW_PRESS) {
        mod_x += 1.0f;
    }

    if (key == GLFW_KEY_A &&
        action == GLFW_PRESS) {
        mod_x -= 1.0f;
    }

    if (key == GLFW_KEY_S) {
        y_mod -= 1.0f;
    }

    if (key == GLFW_KEY_W) {
        y_mod += 1.0f;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float screenWidth = 750.0f;
    float screenHeight = 750.0f;




    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Ina Evangelista", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //Initialize GLAD; should always come after glfwMakeContextCurrent
    gladLoadGL();

    /////////

    std::string path = "3D/submarine.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    stbi_set_flip_vertically_on_load(true);
    int img_width, img_height, color_channels;
    unsigned char* tex_bytes = stbi_load("3D/sub_tex.png",
        &img_width,
        &img_height,
        &color_channels, 0);

    GLuint texture;
    glGenTextures(0, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, //GL_RGB = jpegs / pngs w/o a
        //GL_RGBA = png / images w a
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);
    ///

    //int img_width, img_height, color_channels;
    unsigned char* tex_bytes2 = stbi_load("3D/SubmarineUV.jpg",
        &img_width,
        &img_height,
        &color_channels, 0);

    GLuint texture2;
    glGenTextures(2, &texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB, //GL_RGB = jpegs / pngs w/o a
        //GL_RGBA = png / images w a
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes2
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes2);

    ///

    //int img_width, img_height, color_channels;
    unsigned char* norm_bytes = stbi_load("3D/metal_normal.png",
        &img_width,
        &img_height,
        &color_channels, 0);

    GLuint norm_tex;
    glGenTextures(1, &norm_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, norm_tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, //GL_RGB = jpegs / pngs w/o a
        //GL_RGBA = png / images w a
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        norm_bytes
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(norm_bytes);

    ///

    glEnable(GL_DEPTH_TEST);

    //glViewport(, 0, screenWidth, screenHeight);
    //glViewport(320, 0, 320, 480);

    glfwSetKeyCallback(window, Key_Callback);

    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertString = vertBuff.str();
    const char* v = vertString.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragString = fragBuff.str();
    const char* f = fragString.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    std::fstream skybox_vertSrc("Shaders/skybox.vert");
    std::stringstream skybox_vertBuff;
    skybox_vertBuff << skybox_vertSrc.rdbuf();
    std::string skybox_vertString = skybox_vertBuff.str();
    const char* skybox_v = skybox_vertString.c_str();

    std::fstream skybox_fragSrc("Shaders/skybox.frag");
    std::stringstream skybox_fragBuff;
    skybox_fragBuff << skybox_fragSrc.rdbuf();
    std::string skybox_fragString = skybox_fragBuff.str();
    const char* skybox_f = skybox_fragString.c_str();

    GLuint skybox_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(skybox_vertexShader, 1, &skybox_v, NULL);
    glCompileShader(skybox_vertexShader);

    GLuint skybox_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(skybox_fragmentShader, 1, &skybox_f, NULL);
    glCompileShader(skybox_fragmentShader);

    GLuint skybox_shaderProgram = glCreateProgram();
    glAttachShader(skybox_shaderProgram, skybox_vertexShader);
    glAttachShader(skybox_shaderProgram, skybox_fragmentShader);

    glLinkProgram(skybox_shaderProgram);

    /*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
//Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    
        std::string facesSkybox[]{
            "Skybox_UW/whirlpool_rt.png", // right
            "Skybox_UW/whirlpool_lf.png", // left
            "Skybox_UW/whirlpool_up.png", // up
            "Skybox_UW/whirlpool_dn.png", // down
            "Skybox_UW/whirlpool_ft.png", // front
            "Skybox_UW/whirlpool_bk.png"  // back
        };
    
    

    unsigned int skyboxTex;

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    //Prevents pixelating
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Prevents tiling
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(facesSkybox[i].c_str(),
            &w,
            &h,
            &skyCChannel,
            0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
        }
    }

    stbi_set_flip_vertically_on_load(true);


    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        //V1
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        //V2
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
        //V3
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

        //v1
        glm::vec3 v1 = glm::vec3(
            attributes.vertices[vData1.vertex_index * 3],
            attributes.vertices[vData1.vertex_index * 3 + 1],
            attributes.vertices[vData1.vertex_index * 3 + 2]
        );

        //v2
        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[vData2.vertex_index * 3 + 1],
            attributes.vertices[vData2.vertex_index * 3 + 2]
        );

        //v3
        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[vData3.vertex_index * 3 + 1],
            attributes.vertices[vData3.vertex_index * 3 + 2]
        );

        //UV
        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[vData1.texcoord_index * 2],
            attributes.texcoords[vData1.texcoord_index * 2 + 1]
        );

        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[vData2.texcoord_index * 2],
            attributes.texcoords[vData2.texcoord_index * 2 + 1]
        );

        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[vData3.texcoord_index * 2],
            attributes.texcoords[vData3.texcoord_index * 2 + 1]
        );

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);

    }

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        int vertexIndex = vData.vertex_index * 3;
        int uvIndex = vData.texcoord_index * 2;


        //X
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        //Y
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        //Z
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        //U
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        //V
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

        fullVertexData.push_back(
            tangents[i].x
        );

        fullVertexData.push_back(
            tangents[i].y
        );

        fullVertexData.push_back(
            tangents[i].z
        );

        fullVertexData.push_back(
            bitangents[i].x
        );

        fullVertexData.push_back(
            bitangents[i].y
        );

        fullVertexData.push_back(
            bitangents[i].z
        );

    }

    GLfloat vertices[]{
        0.f, 0.5f, 0.f,     //0
        -0.5f, -0.5f, 0.f,  //1
        0.5f, -0.5f, 0.f    //2
    };

    GLuint indices[]{
        0,1,2 //Triangle 1
    };

    GLuint VAO, VBO;
    //Generate and Assign ID to VAO
    glGenVertexArrays(1, &VAO);
    //Generate and Assign ID to VBO
    glGenBuffers(1, &VBO);



    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData.size(), 
        fullVertexData.data(), 
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, //Position
        3, //XYZ
        GL_FLOAT,
        GL_FALSE,
        //XYZ UV so 5 // asignment 3 : 8 bc of adding 3 normals (5(xyzuv) + 3(normals))
        14 * sizeof(GL_FLOAT), //How much 3 is in float
        (void*)0);
        
    GLintptr normPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)normPtr

    );

    GLintptr uvPtr = 6 * sizeof(GL_FLOAT);
    
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)uvPtr
    );

    GLintptr tangentPtr = 8 * sizeof(float);
    GLintptr bitangentPtr = 11 * sizeof(float); 

    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)tangentPtr
    );

    glVertexAttribPointer(
        4,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)bitangentPtr
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /////////////////////////////

    //3x3 Identity Matrix
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    //4x4 Identity Matrix
    glm::mat4 identity_matrix = glm::mat4(1.0f);
    glm::mat4 identity_matrix2 = glm::mat4(1.0f);

    float x, y, z;
    x = y = z = 0.0f;
    //y = -0.5f;
    z = 0.0f;

    float scale_x, scale_y, scale_z, scale_x2, scale_y2, scale_z2;
    scale_x = scale_y = scale_z = 0.8f;
    scale_x2 = scale_y2 = scale_z2 = 3.1f;

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0;
    rot_x = 1.0f;

    float theta = -180.0f;

    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(60.0f), // FOV
        screenHeight / screenWidth, // aspect ratio
        0.1f, // near
        100.0f // far
    );

    glm::vec3 lightPos = glm::vec3(-5, 5, 0);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    float ambientStr = 0.8f;
    glm::vec3 ambientColor = glm::vec3(1, 1, 1);

    float specStr = 2.f;
    float specPhong = 2.0f; 

    

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glEnable(GL_BLEND);
    
        //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_ALPHA);
        //glBlendEquation(GL_FUNC_SUBTRACT);

        //
        //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
        //glBlendEquation(GL_FUNC_SUBTRACT);

        //assignment 5?
        //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_DST_COLOR);
        //glBlendEquation(GL_FUNC_SUBTRACT);

        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        lightPos.z = mod_x;

        //theta = mod_x;
        //z = z_mod;
        theta += 0.01f;

        glm::vec3 cameraPos = glm::vec3(0, 0, 10.0f);
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 cameraCenter = glm::vec3(mod_x, y_mod, 0);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, WorldUp);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(skybox_shaderProgram);

        glm::mat4 sky_view = glm::mat4(1.f);

        sky_view = glm::mat4(glm::mat3(viewMatrix));

        unsigned int sky_projectionLoc = glGetUniformLocation(skybox_shaderProgram, "projection");
        glUniformMatrix4fv(sky_projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        unsigned int sky_viewLoc = glGetUniformLocation(skybox_shaderProgram, "view");
        glUniformMatrix4fv(sky_viewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);

        glUseProgram(shaderProgram);

        //////////////////////////

        glm::mat4 transformation_matrix = glm::mat4(1.0f);
        //Translation
        transformation_matrix = glm::translate(identity_matrix,
            glm::vec3(x, y, z + -10.0f));
        //Scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));
        //Rotation
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x , rot_y, rot_z))
        );

        unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        unsigned int ambientStrLoc = glGetUniformLocation(shaderProgram, "ambientStr");
        glUniform1f(ambientStrLoc, ambientStr);

        unsigned int ambientColorLoc = glGetUniformLocation(shaderProgram, "ambientColor");
        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));

        unsigned int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

        unsigned int specStrLoc = glGetUniformLocation(shaderProgram, "specStr");
        glUniform1f(specStrLoc, specStr);

        unsigned int specPhongLoc = glGetUniformLocation(shaderProgram, "specPhong");
        glUniform1f(specPhongLoc, specPhong);
      
        glActiveTexture(GL_TEXTURE0);
        GLuint texOAddress = glGetUniformLocation(shaderProgram, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texOAddress, 0);

        //glActiveTexture(GL_TEXTURE2);
        //GLuint tex2Address = glGetUniformLocation(shaderProgram, "tex2");
        //glBindTexture(GL_TEXTURE_2D, texture2);
        //glUniform1i(tex2Address, 2);
       
        glActiveTexture(GL_TEXTURE1);
        GLuint tex1Address = glGetUniformLocation(shaderProgram, "norm_tex");
        glBindTexture(GL_TEXTURE_2D, norm_tex);
        glUniform1i(tex1Address, 1); 

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int transformationLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE,
            glm::value_ptr(transformation_matrix));

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 14);

        ////////////////////////

        /*
        
        //Translation
        transformation_matrix = glm::translate(identity_matrix,
            glm::vec3(x + 2.5f, y, z));
        //Scale
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));
        //Rotation
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z))
        );

        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        glUniform1f(ambientStrLoc, ambientStr);

        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));

        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

        glUniform1f(specStrLoc, specStr);

        glUniform1f(specPhongLoc, specPhong);

        //glBindTexture(GL_TEXTURE_2D, texture);
        //glUniform1i(texOAddress, 0);

        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(tex2Address, 2);

        glBindTexture(GL_TEXTURE_2D, norm_tex);
        glUniform1i(tex1Address, 1);

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE,
            glm::value_ptr(transformation_matrix));
        */


        ////////////////////////


        //glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 5);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);


    glfwTerminate();
    return 0;
}