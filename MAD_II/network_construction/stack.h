#include <vector>
template <typename T>
class Stack
{
  private:
    std::vector<T> data;
    size_t top;

  public:
    Stack()
    {
        this->top = -1;
    }

    ~Stack()
    {
        this->data.clear();
        top = -1;
    }
    void push(const T value)
    {
        this->data.push_back(value);
        ++this->top;
    }

    T pop()
    {
        T element = data[this->top--];
        this->data.pop_back();
        return element;
    }

    T peek() const
    {
        return this->data[this->top];
    }

    operator bool() const
    {
        return ((int)this->top > -1);
    }
};
