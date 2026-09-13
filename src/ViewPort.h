
#ifndef LEVELEDITOR_VIEWPORT_H
#define LEVELEDITOR_VIEWPORT_H

#include <vector>
#include <functional>
#include <string>

class ViewPortBehaviour
{
public:
    virtual ~ViewPortBehaviour() = default;
    virtual void Render(float x, float y, float width, float height, int id) = 0;
};

struct ViewPortBehaviourSelector
{
    std::string name;
    std::function<ViewPortBehaviour*()> func;
};

class ViewPort
{
private:
    int current_selected = 0;
    static const std::vector<ViewPortBehaviourSelector>& GetAllBehaviours();
    ViewPortBehaviour* _behaviour = nullptr;
public:
    int Id;
    explicit ViewPort(int id);
    void Render(float x, float y, float width, float height);
    ViewPortBehaviour* GetBehaviour();
};


#endif //LEVELEDITOR_VIEWPORT_H
