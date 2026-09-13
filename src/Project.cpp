
#include "Project.h"
#include <cmath>

Project* Project::_current;

Project* Project::GetCurrent()
{
    return _current;
}

void Project::CreateNew()
{
    delete _current;
    _current = new Project();
}

float Project::GetCellSize()
{
    if(cell_multiplier < -2) cell_multiplier = -2;
    float result = cell_multiplier;
    if(result < 0)
    {
        result = 1.0f / ((float)abs( cell_multiplier * 2 ));
    }
    else
    {
        //result *= 1;
        //if(result == 0) result = 1;
        result++;
    }
    return result;
}

glm::vec3 Project::SnapToGrid(glm::vec3 pos)
{
    float cell = GetCellSize();
    pos /= cell;
    pos.x = ceilf(pos.x);
    pos.y = ceilf(pos.y);
    pos.z = ceilf(pos.z);
    pos *= cell;
    return pos;
}

void Project::ApplyAction(Action* action)
{
    if(_currentAction > 0)
    {
        for(auto it = _actions.end() - _currentAction + 1; it != _actions.end(); ++it)
        {
            delete it.base();
        }
        _actions.erase(_actions.end() - _currentAction + 1,_actions.end());
    }
    _currentAction = 0;
    action->Apply();
    _actions.push_back(action);
}

void Project::RevertAction()
{
    if(_actions.empty()) return;
    _currentAction++;
    _actions[_actions.size() - _currentAction]->Revert();
}

void Project::RenderAction()
{
    if(_actions.empty()) return;
    _actions[_actions.size() - 1 - _currentAction]->DrawMenu();
}

std::string Project::GetActionName()
{
    if(_actions.empty()) return "";
    return _actions[_actions.size() - 1 - _currentAction]->GetName();
}
