#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <rapidjson/document.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace rapidjson;

void fatalError(const string &message)
{
    cout << message << endl;
    glfwTerminate();
    exit(-1);
}

GLuint createTexture(const char *path)
{
    int h_image, w_image, cnt;
    unsigned char *data = stbi_load(path, &w_image, &h_image, &cnt, 0);
    if (data == NULL)
    {
        cout << "Failed to load an image" << endl;
        glfwTerminate();
        exit(-1);
    }

    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_image, h_image, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

void appendFrameTo(vector<float> &texCoords, const Document &doc, const string &frameName,
                   float imageW, float imageH, map<string, int> &drawIndices)
{
    drawIndices[frameName] = drawIndices.size() * 4;

    auto frame = doc["frames"][(frameName + ".png").c_str()]["frame"].GetObject();

    float x = frame["x"].GetFloat();
    float y = frame["y"].GetFloat();
    float w = frame["w"].GetFloat();
    float h = frame["h"].GetFloat();

    // vertex 0
    texCoords.push_back(x / imageW);
    texCoords.push_back(y / imageH);
    // vertex 1
    texCoords.push_back(x / imageW);
    texCoords.push_back((y + h) / imageH);
    // vertex 2
    texCoords.push_back((x + w) / imageW);
    texCoords.push_back(y / imageH);
    // vertex 3
    texCoords.push_back((x + w) / imageW);
    texCoords.push_back((y + h) / imageH);
}

int main()
{
    if (!glfwInit())
        fatalError("Failed to init GLFW");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const int winW = 350;
    const int winH = 350;
    GLFWwindow *window = glfwCreateWindow(winW, winH, "GLFW, OpenGL 1.1, C++", nullptr, nullptr);
    if (!window)
        fatalError("Failed to create the GLFW window");
    glfwMakeContextCurrent(window);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, winW, winH);
    glClearColor(0.f, 0.54f, 0.77f, 1.f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winW, winH, 0, -100.f, 100.f);

    // Read a file content
    string spritesFilePath = "assets/sprites/texture.json";
    ifstream f(spritesFilePath);
    if (!f.is_open())
        fatalError("Failed to open the file: " + spritesFilePath);
    stringstream sstr;
    f >> sstr.rdbuf();
    string content = sstr.str();
    f.close();

    // Parse the content
    Document doc;
    doc.Parse(content.c_str());
    float imageW = doc["meta"]["size"]["w"].GetFloat();
    float imageH = doc["meta"]["size"]["h"].GetFloat();

    vector<float> texCoords;
    map<string, int> drawIndices;
    appendFrameTo(texCoords, doc, "coin", imageW, imageH, drawIndices);
    appendFrameTo(texCoords, doc, "mario_idle", imageW, imageH, drawIndices);

    size_t amountOfSprites = texCoords.size() / 8;
    vector<float> vertPositions;
    for (size_t i = 0; i < amountOfSprites; ++i)
    {
        vertPositions.push_back(-0.5f);
        vertPositions.push_back(-0.5f);
        vertPositions.push_back(0.f);
        vertPositions.push_back(-0.5f);
        vertPositions.push_back(0.5f);
        vertPositions.push_back(0.f);
        vertPositions.push_back(0.5f);
        vertPositions.push_back(-0.5f);
        vertPositions.push_back(0.f);
        vertPositions.push_back(0.5f);
        vertPositions.push_back(0.5f);
        vertPositions.push_back(0.f);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    const char *texturePath = "assets/sprites/texture.png";
    GLuint texture = createTexture(texturePath);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glVertexPointer(3, GL_FLOAT, 0, vertPositions.data());
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords.data());
        glBindTexture(GL_TEXTURE_2D, texture);

        // Player
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(120.f, 150.f, 0.f);
        glScalef(100.f, 100.f, 1.f);
        glDrawArrays(GL_TRIANGLE_STRIP, drawIndices["mario_idle"], 4);

        // Coin
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(210.f, 150.f, 0.f);
        glScalef(100.f, 100.f, 1.f);
        glDrawArrays(GL_TRIANGLE_STRIP, drawIndices["coin"], 4);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
