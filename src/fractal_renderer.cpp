#include "fractal_renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

FractalRenderer::FractalRenderer(int viewportWidth, int viewportHeight) {
    width_ = viewportWidth;
    height_ = viewportHeight;
    textureImgBuffer_ = (GLubyte* )malloc(sizeof(GLubyte) * COLOR_DEPTH * width_ * height_);

    initializeShaders();
    initializeScreenQuad();
    initializeScreenTexture();
    initializePaletteTexture("res/palette.png");
}

FractalRenderer::~FractalRenderer() {
    free(textureImgBuffer_);
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
}

void FractalRenderer::draw(ParamInput& paramInput) {
    auto begin = chrono::high_resolution_clock::now();

    double range = paramInput.getRange();
    complex<double> start = paramInput.getOrigin() - complex<double>(range / 2.0, range / 2.0);
    complex<double> delta = complex<double>(range / width_, range / height_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture_);
    //renderToTexture(start, delta, paramInput.getMaxIters());

    setFragmentShaderParams(start, delta, paramInput.getMaxIters());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, paletteTexture_);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsed = end - begin;
    cout << "draw(): " << elapsed.count() << " ms" << endl;
}

void FractalRenderer::setFragmentShaderParams(complex<double> start, complex<double> delta, int maxIterations) {
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "gpuMode"), 1);
    glUniform2d(glGetUniformLocation(shaderProgram_->getId(), "start"), start.real(), start.imag());
    glUniform2d(glGetUniformLocation(shaderProgram_->getId(), "delta"), delta.real(), delta.imag());
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "maxIterations"), maxIterations);
}

void FractalRenderer::renderToTexture(complex<double> start, complex<double> delta, int maxIterations) {
    vector<thread> threads;
    const int numThreads = thread::hardware_concurrency();
    const int chunkSize = height_ / numThreads;

    memset(textureImgBuffer_, 0x66, sizeof(GLubyte) * COLOR_DEPTH * width_ * height_);

    int remainingLines = height_;
    for (int i = numThreads ; i > 0 ; i--) {
        int fromLine = (i > 1) ? remainingLines - chunkSize : 0;
        threads.push_back(thread(
            &FractalRenderer::renderLinesToBuffer,
            this,
            ref(textureImgBuffer_),
            fromLine,
            remainingLines,
            start,
            delta,
            maxIterations
        ));

        remainingLines -= chunkSize;
    }

    for (auto &t : threads) {
        t.join();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImgBuffer_);
}

void FractalRenderer::renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, complex<double> start, complex<double> delta, int maxIterations) {
    double re, im;
    int iterations = 0;

    for (int y = fromLine ; y < toLine ; y++) {
        im = start.imag() + delta.imag() * y;
        for (int x = 0 ; x < width_ ; x++) {
            re = start.real() + delta.real() * x;
            iterations = mandelbrot(re, im, maxIterations);

            int offset = (y * width_ + x) * COLOR_DEPTH;
            setColor(&buffer[offset], iterations, maxIterations);
        }
    }
}

inline int FractalRenderer::mandelbrot(double cRe, double cIm, int maxIter) {
    double zRe = 0.0;
    double zIm = 0.0;
    double zReSq = 0.0;
    double zImSq = 0.0;
    int iter = 0;

    while (zReSq + zImSq < ESCAPE_RADIUS && iter < maxIter) {
        zReSq = zRe * zRe;
        zImSq = zIm * zIm;

        zIm = zRe * zIm;
        zIm += zIm; // same as x2, multiplication is expensive
        zIm += cIm;
        zRe = zReSq - zImSq + cRe;

        iter++;
    }

    return iter;
}

void FractalRenderer::setColor(GLubyte* pixel, int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        memset(pixel, 0x00, 3);
        return;
    }

    float normIters = (float)iterations / maxIterations;

    pixel[0] = (int)(pow(normIters, 0.8) * 255);
    pixel[1] = (int)(pow(normIters, 1.0) * 255);
    pixel[2] = (int)(pow(normIters, 0.5) * 255);
}

void FractalRenderer::initializeShaders() {
    shaderProgram_ = new ShaderProgram("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    shaderProgram_->link();
    shaderProgram_->use();
}

void FractalRenderer::initializeScreenQuad() {
    float quadVertices[20] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    int quadIndices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FractalRenderer::initializeScreenTexture() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &screenTexture_);
    glBindTexture(GL_TEXTURE_2D, screenTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "screenTexture"), 0);
}

void FractalRenderer::initializePaletteTexture(const char* path) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &paletteTexture_);
    glBindTexture(GL_TEXTURE_1D, paletteTexture_);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    if (data) {
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        cerr << "Failed to load palette texture from " << path << endl;
    }

    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "paletteTexture"), 1);
}