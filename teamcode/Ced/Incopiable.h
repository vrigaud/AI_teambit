#ifndef INCOPIABLE_H
#define INCOPIABLE_H

struct Incopiable {
   Incopiable(const Incopiable&) = delete;
   Incopiable& operator=(const Incopiable&) = delete;
protected:
   Incopiable() = default;
   ~Incopiable() = default;
};

#endif
