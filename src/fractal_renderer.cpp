#include "fractal_renderer.h"

FractalRenderer::FractalRenderer(int viewportWidth, int viewportHeight) {
    width_ = viewportWidth;
    height_ = viewportHeight;
    textureImgBuffer_ = (GLubyte* )malloc(sizeof(GLubyte) * COLOR_DEPTH * width_ * height_);

    initializeShaders();
    initializeQuad();
    initializeTexture();
}

FractalRenderer::~FractalRenderer() {
    free(textureImgBuffer_);
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
}

void FractalRenderer::draw(ParamInput& paramInput) {
    auto start = std::chrono::high_resolution_clock::now();

    //renderToTexture(paramInput.getOriginRe(), paramInput.getOriginIm(), paramInput.getRange(), paramInput.getMaxIters());
    setFragmentShaderParams(paramInput.getOriginRe(), paramInput.getOriginIm(), paramInput.getRange(), paramInput.getMaxIters());

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "draw(): " << elapsed.count() << " ms" << std::endl;
}

void FractalRenderer::setFragmentShaderParams(double originRe, double originIm, double range, int maxIterations) {
    double reBase = originRe - range / 2.0;
    double imBase = originIm - range / 2.0;
    double reDelta = range / width_;
    double imDelta = range / height_;

    int baseLocation = glGetUniformLocation(shaderProgram_->programId_, "base");
    glUniform2d(baseLocation, reBase, imBase);
    int deltaLocation = glGetUniformLocation(shaderProgram_->programId_, "delta");
    glUniform2d(deltaLocation, reDelta, imDelta);
    int maxIterationsLocation = glGetUniformLocation(shaderProgram_->programId_, "maxIterations");
    glUniform1i(maxIterationsLocation, maxIterations);
}

void FractalRenderer::renderToTexture(double originRe, double originIm, double range, int maxIterations) {
    std::vector<std::thread> threads;
    const int numThreads = std::thread::hardware_concurrency();
    const int chunkSize = height_ / numThreads;

    memset(textureImgBuffer_, 0x66, sizeof(GLubyte) * COLOR_DEPTH * width_ * height_);

    int remainingLines = height_;
    for (int i = numThreads ; i > 0 ; i--) {
        int fromLine = (i > 1) ? remainingLines - chunkSize : 0;
        threads.push_back(std::thread(
            &FractalRenderer::renderLinesToBuffer,
            this,
            std::ref(textureImgBuffer_),
            fromLine,
            remainingLines,
            originRe,
            originIm,
            range,
            maxIterations
        ));

        remainingLines -= chunkSize;
    }

    for (auto &t : threads) {
        t.join();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImgBuffer_);
}

void FractalRenderer::renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, double originRe, double originIm, double range, int maxIterations) {
    double re, im;
    double reBase = originRe - range / 2.0;
    double imBase = originIm - range / 2.0;
    double reDelta = range / width_;
    double imDelta = range / height_;
    int iterations = 0;

    for (int y = fromLine ; y < toLine ; y++) {
        im = imBase + imDelta * y;
        for (int x = 0 ; x < width_ ; x++) {
            re = reBase + reDelta * x;
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

void FractalRenderer::initializeQuad() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices_), quadVertices_, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices_), quadIndices_, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FractalRenderer::initializeTexture() {
    GLuint texture;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}