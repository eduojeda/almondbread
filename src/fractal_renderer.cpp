#include "fractal_renderer.h"

inline int FractalRenderer::mandelbrot(double cRe, double cIm, int maxModSq, int maxIter) {
    double zRe = 0.0;
    double zIm = 0.0;
    double zReSq = 0.0;
    double zImSq = 0.0;
    int iter = 0;

    while (zReSq + zImSq < maxModSq && iter < maxIter) {
        zReSq = zRe * zRe;
        zImSq = zIm * zIm;
        zIm = 2.0 * zRe * zIm + cIm;
        zRe = zReSq - zImSq + cRe;
        iter++;
    }

    return iter;
}

FractalRenderer::FractalRenderer(int viewportWidth, int viewportHeight) {
    width_ = viewportWidth;
    height_ = viewportHeight;
    textureImgBuffer_ = (GLubyte* )malloc(sizeof(GLubyte) * colorDepth_ * width_ * height_);

    initShaders();
    initQuad();
    initTexture();
}

FractalRenderer::~FractalRenderer() {
    free(textureImgBuffer_);
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
}

void FractalRenderer::draw(ParamInput& paramInput) {
    auto start = std::chrono::high_resolution_clock::now();

    generateTextureImage(
        textureImgBuffer_,
        paramInput.getOriginRe(),
        paramInput.getOriginIm(),
        paramInput.getRange()
    );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImgBuffer_);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "draw(): " << elapsed.count() << " ms" << std::endl;
}

void FractalRenderer::initShaders() {
    ShaderLoader shaderLoader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    glUseProgram(shaderLoader.createProgram());
}

void FractalRenderer::initQuad() {
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

void FractalRenderer::initTexture() {
    GLuint texture;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void FractalRenderer::generateTextureImage(GLubyte* buffer, double originRe, double originIm, double range) {
    std::vector<std::thread> threads;
    const int numThreads = std::thread::hardware_concurrency();
    const int chunkSize = height_ / numThreads;

    memset(buffer, 0x66, sizeof(GLubyte) * colorDepth_ * width_ * height_);

    int remainingLines = height_;
    for (int i = numThreads ; i > 0 ; i--) {
        int fromLine = (i > 1) ? remainingLines - chunkSize : 0;
        threads.push_back(std::thread(
            &FractalRenderer::renderLinesToBuffer,
            this,
            std::ref(buffer),
            fromLine,
            remainingLines,
            originRe,
            originIm,
            range
        ));

        remainingLines -= chunkSize;
    }

    for (auto &t : threads) {
        t.join();
    }
}

void FractalRenderer::renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, double originRe, double originIm, double range) {
    double re, im;
    double reBase = originRe - range / 2.0;
    double imBase = originIm - range / 2.0;
    double reDelta = range / width_;
    double imDelta = range / height_;
    int iterations = 0;
    int maxIterations = (quality_ / range) + quality_;

    for (int y = fromLine ; y < toLine ; y++) {
        im = imBase + imDelta * y;
        for (int x = 0 ; x < width_ ; x++) {
            re = reBase + reDelta * x;
            iterations = mandelbrot(re, im, 4, maxIterations);

            int offset = (y * width_ + x) * colorDepth_;
            setColor(&buffer[offset], iterations, maxIterations);
        }
    }
}

void FractalRenderer::setColor(GLubyte* pixel, int iterations, int maxIterations) {
    float normIters = (float)iterations / maxIterations;
    pixel[0] = (int)(pow(normIters, 0.8) * 255);
    pixel[1] = (int)(pow(normIters, 1.0) * 255);
    pixel[2] = (int)(pow(normIters, 0.5) * 255);
}