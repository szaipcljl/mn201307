#!/usr/bin/env python
# coding=utf-8

# Filename:inherit.py

class SchoolMember:
    '''Represents any school member'''
    def __init__(self, name, age):
        self.name = name
        self.age = age
        print '(Initialized SchoolMember: %s)' % self.name

    def tell(self):
        '''Tell my details.'''
        print 'Name:"%s" Age:"%s"' % (self.name, self.age),

class Teacher(SchoolMember):
    '''Represents a teacher'''
    def __init__(self, name, age,salary):
        SchoolMember.__init__(self, name, age)
        self.salary = salary
        print '(Initialized Teacher:%s)' % self.name

    def tell(self):
        SchoolMember.tell(self)
        print 'Salary:"%d"' % self.salary

class Student(SchoolMember):
    '''Represents a student.'''
    def __init__(self, name, age, marks):
        SchoolMember.__init__(self, name, age)
        self.marks = marks
        print '(Initialized Student: %s)' % self.name

    def tell(self):
        SchoolMember.tell(self)
        print 'Marks:"%d"' % self.marks

t = Teacher('Mrs. Shrividya', 40, 30000)
s = Student('Swaroop', 22, 75)

print #prints a blank line

members = [t,s]
for member in members:
    member.tell() #works for both Teachers and Students

# note:
# 为了使用继承，我们把基本类的名称作为一个元组跟在定义类时的类名称之后。然后，我们注
# 意到基本类的__init__方法专门使用self变量调用，这样我们就可以初始化对象的基本类部分。
# 这一点十分重要——Python不会自动调用基本类的constructor，你得亲自专门调用它。
# 在方法调用之前加上类名称前缀，然后把self变量及其他参数传递给它。
# 使用SchoolMember类的tell方法的时候，我们把Teacher和Student的实例仅仅作为
# SchoolMember的实例

# 我们调用了子类型的tell方法，而不是SchoolMember类的tell方法。可以
# 这样来理解，Python总是首先查找对应类型的方法，在这个例子中就是如此。如果它不能在导
# 出类中找到对应的方法，它才开始到基本类中逐个查找。基本类是在类定义的时候，在元组之
# 中指明的
#
# 如果在继承元组中列了一个以上的类，那么它就被称作 多重继承 。
