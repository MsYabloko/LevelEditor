#ifndef LEVELEDITOR_FRAMEBUFFER_H
#define LEVELEDITOR_FRAMEBUFFER_H

class Framebuffer
{
private:
    unsigned int fbo;
    unsigned int texture;
    unsigned int rbo;
public:
    Framebuffer(int width, int height);
    ~Framebuffer();
    void use() const;
    unsigned int get_texture() const;
    static void stop();
};

#endif //LEVELEDITOR_FRAMEBUFFER_H