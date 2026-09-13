
#ifndef LEVELEDITOR_SHADER_H
#define LEVELEDITOR_SHADER_H


class Shader
{
private:
    unsigned int Id;
public:
    void use() const;
    static Shader* load(const char* vertexPath, const char* fragmentPath);
    [[nodiscard]] unsigned int getId() const;
    Shader(const char* vertex, const char* fragment);
    ~Shader();
    static void stop();
};


#endif //LEVELEDITOR_SHADER_H
