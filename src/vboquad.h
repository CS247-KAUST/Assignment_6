#ifndef VBOQuad_H
#define VBOQuad_H

class VBOQuad
{

private:
    unsigned int vaoHandle;

public:
    VBOQuad();

    void init();
    void render();
};

#endif // VBOQuad_H
