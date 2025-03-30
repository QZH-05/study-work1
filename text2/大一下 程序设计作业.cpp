#include <iostream>
#include <vector>  //动态数组实现容器。作用：提供可变大小数组实现。特性：自动内存管理（动态扩容），随机访问迭代器（支持下标访问），尾部插入/删除高效
#include <fstream> //文件流操作。作用：实现文件输入/输出功能。核心类：ifstream(读取文件),ofstream(写入文件),fstream(双向文件流)
#include <memory> //智能指针。作用实现安全的内存管理
#include <algorithm>//通用算法
#include <sstream>//字符串流的处理
#include <string>

using namespace std;

// 小时转换类
class HourConverter {
public:
    static double hoursToDays(int hours) {
        if (hours <= 8) return hours / 8.0;
        return (hours / 12.0) * 1.5;
    }

    static double add(double days1, double days2) { return days1 + days2; }
    static double sub(double days1, double days2) { return days1 - days2; }
};

// 休假记录基类
class LeaveRecord {
protected:
    string employeeId;
    int sickHours = 0; //生病时间
    int paidLeaveHours = 0;  //带薪休假时间

public:
    LeaveRecord(const string& id) : employeeId(id) {}

    int getSickHours() const { return sickHours; }
    int getPaidLeaveHours() const { return paidLeaveHours; }

    bool addSickHours(int hours) {
        if (sickHours + hours > 16 || sickHours + hours < 0) return false;
        sickHours += hours;
        return true;
    }

    bool addPaidLeave(int hours) {
        if (paidLeaveHours + hours > 24 || paidLeaveHours + hours < 0) return false;
        paidLeaveHours += hours;
        return true;
    }

    void setSickHours(int hours) {
        if (hours >= 0 && hours <= 16)
            sickHours = hours;
        else
            cerr << "错误：病假小时超出范围！" << endl;
    }

    void setPaidLeaveHours(int hours) {
        if (hours >= 0 && hours <= 24)
            paidLeaveHours = hours;
        else
            cerr << "错误：带薪假小时超出范围！" << endl;
    }

    void display() const {
        cout << "病假小时: " << sickHours << " (最大16小时)" << endl;
        cout << "带薪假小时: " << paidLeaveHours << " (最大24小时)" << endl;
    }

    friend class Employee;
};

// 抽象员工基类
class Employee {
protected:
    string id;
    string name;
    string gender;
    string department;
    int level;
    unique_ptr<LeaveRecord> leaveRecord;

public:
    Employee(const string& id, const string& name, const string& gender,
        const string& dept, int lvl)
        : id(id), name(name), gender(gender), department(dept), level(lvl),
        leaveRecord(make_unique<LeaveRecord>(id)) {
    }

    virtual ~Employee() = default;

    int getSickHours() const { return leaveRecord->getSickHours(); }
    int getPaidLeaveHours() const { return leaveRecord->getPaidLeaveHours(); }
    LeaveRecord* getLeaveRecord() const { return leaveRecord.get(); }

    bool addSickHours(int hours) { return leaveRecord->addSickHours(hours); }
    bool addPaidLeave(int hours) { return leaveRecord->addPaidLeave(hours); }
    void setSickHours(int hours) { leaveRecord->setSickHours(hours); }
    void setPaidLeaveHours(int hours) { leaveRecord->setPaidLeaveHours(hours); }

    virtual double calculateSalary() const = 0;

    virtual void display() const {
        cout << "ID: " << id << "\n姓名: " << name << "\n性别: " << gender
            << "\n部门: " << department << "\n级别: " << level << endl;
        ;
    }

    virtual void saveToFile(ofstream& file) const {
        file << id << "," << name << "," << gender << ","
            << department << "," << level << ","
            << leaveRecord->getSickHours() << ","
            << leaveRecord->getPaidLeaveHours();
    }

    string getId() const { return id; }
};
//经理类
class Manager : public Employee {
protected:
    double fixedSalary;

public:
    Manager(const string& id, const string& name, const string& gender,
        const string& dept, int lvl, double salary)
        : Employee(id, name, gender, dept, lvl), fixedSalary(salary) {
    }

    double calculateSalary() const override {
        return fixedSalary - (HourConverter::hoursToDays(getSickHours()) * 100);
    }

    void display() const override {
        Employee::display();
        cout << "职位: 经理\n固定工资: " << fixedSalary
            << "\n实发工资: " << calculateSalary() << endl;
    }

    void saveToFile(ofstream& file) const override {
        file << "M,";
        Employee::saveToFile(file);
        file << "," << fixedSalary << "\n";
    }
};
//技术员类
class Technician : public Employee {
protected:
    double hourlyRate;
    int workHours;

public:
    Technician(const string& id, const string& name, const string& gender,
        const string& dept, int lvl, double rate, int hours)
        : Employee(id, name, gender, dept, lvl), hourlyRate(rate), workHours(hours) {
    }

    double calculateSalary() const override {
        return (hourlyRate * workHours) - (HourConverter::hoursToDays(getSickHours()) * 50);
    }

    void display() const override {
        Employee::display();
        cout << "职位: 技术人员\n时薪: " << hourlyRate
            << "\n工作小时: " << workHours
            << "\n实发工资: " << calculateSalary() << endl;
    }

    void saveToFile(ofstream& file) const override {
        file << "T,";
        Employee::saveToFile(file);
        file << "," << hourlyRate << "," << workHours << "\n";
    }
};
//销售员类
class Salesman : public Employee {
    double baseSalary;
    double salesAmount;
    double commissionRate;

public:
    Salesman(const string& id, const string& name, const string& gender,
        const string& dept, int lvl, double base, double sales, double rate)
        : Employee(id, name, gender, dept, lvl),
        baseSalary(base), salesAmount(sales), commissionRate(rate) {
    }

    double calculateSalary() const override {
        return baseSalary + (salesAmount * commissionRate)
            - (HourConverter::hoursToDays(getSickHours()) * 80);
    }

    void display() const override {
        Employee::display();
        cout << "职位: 销售人员\n底薪: " << baseSalary
            << "\n销售额: " << salesAmount
            << "\n提成比例: " << commissionRate
            << "\n实发工资: " << calculateSalary() << endl;
    }

    void saveToFile(ofstream& file) const override {
        file << "S,";
        Employee::saveToFile(file);
        file << "," << baseSalary << "," << salesAmount << "," << commissionRate << "\n";
    }
};

//销售经理类
class SalesManager : public Manager {
    double teamSales;
    double teamCommission;

public:
    SalesManager(const string& id, const string& name, const string& gender,
        const string& dept, int lvl, double salary, double sales, double commission)
        : Manager(id, name, gender, dept, lvl, salary),
        teamSales(sales), teamCommission(commission) {
    }

    double calculateSalary() const override {
        return fixedSalary + (teamSales * teamCommission)
            - (HourConverter::hoursToDays(getSickHours()) * 150);
    }

    void display() const override {
        Employee::display();
        cout << "职位: 销售经理\n固定工资: " << fixedSalary
            << "\n团队销售额: " << teamSales
            << "\n团队提成比例: " << teamCommission
            << "\n实发工资: " << calculateSalary() << endl;
    }

    void saveToFile(ofstream& file) const override {
        file << "SM,";
        Employee::saveToFile(file);
        file << "," << fixedSalary << "," << teamSales << "," << teamCommission << "\n";
    }
};
  //考勤系统类
class AttendanceSystem {
    vector<unique_ptr<Employee>> employees;

public:
    void addEmployee(unique_ptr<Employee> emp) {
        if (findEmployee(emp->getId()) != nullptr) {
            cerr << "错误：员工ID已存在！" << endl;
            return;
        }
        employees.push_back(move(emp));
    }

    Employee* findEmployee(const string& id) {
        auto it = find_if(employees.begin(), employees.end(),
            [&](const unique_ptr<Employee>& e) { return e->getId() == id; });
        return (it != employees.end()) ? it->get() : nullptr;
    }

    bool deleteEmployee(const string& id) {
        auto it = remove_if(employees.begin(), employees.end(),
            [&](const unique_ptr<Employee>& e) { return e->getId() == id; });
        if (it != employees.end()) {
            employees.erase(it, employees.end());
            return true;
        }
        return false;
    }

    void displayAll() const {
        for (const auto& emp : employees) {
            emp->display();
            emp->getLeaveRecord()->display();
            cout << "------------------------" << endl;
        }
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        for (const auto& emp : employees) {
            emp->saveToFile(file);
        }
        cout << "数据保存成功！" << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "无法打开文件！" << endl;
            return;
        }

        employees.clear();
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string type;
            getline(ss, type, ',');

            auto trim = [](string& str) {
                str.erase(str.find_last_not_of(" \n\r\t") + 1); // 去除尾部空白
                str.erase(0, str.find_first_not_of(" \n\r\t")); // 去除首部空白
                };

            if (type == "M") {
                string id, name, gender, dept, levelStr, sickStr, paidStr, salaryStr;
                getline(ss, id, ',');
                getline(ss, name, ',');
                getline(ss, gender, ',');
                getline(ss, dept, ',');
                getline(ss, levelStr, ',');
                getline(ss, sickStr, ',');
                getline(ss, paidStr, ',');
                getline(ss, salaryStr);

                // 去除换行符
                trim(salaryStr);

                auto emp = make_unique<Manager>(id, name, gender, dept, stoi(levelStr), stod(salaryStr));
                emp->setSickHours(stoi(sickStr));
                emp->setPaidLeaveHours(stoi(paidStr));
                employees.push_back(move(emp));
            }
            else if (type == "T") {
                string id, name, gender, dept, levelStr, sickStr, paidStr, rateStr, hoursStr;
                getline(ss, id, ',');
                getline(ss, name, ',');
                getline(ss, gender, ',');
                getline(ss, dept, ',');
                getline(ss, levelStr, ',');
                getline(ss, sickStr, ',');
                getline(ss, paidStr, ',');
                getline(ss, rateStr, ',');
                getline(ss, hoursStr);

                //处理数值型字段
                trim(hoursStr);

                auto emp = make_unique<Technician>(id, name, gender, dept, stoi(levelStr), stod(rateStr), stoi(hoursStr));
                emp->setSickHours(stoi(sickStr));
                emp->setPaidLeaveHours(stoi(paidStr));
                employees.push_back(move(emp));
            }
            else if (type == "S") {
                string id, name, gender, dept, levelStr, sickStr, paidStr, baseStr, salesStr, rateStr;
                getline(ss, id, ',');
                getline(ss, name, ',');
                getline(ss, gender, ',');
                getline(ss, dept, ',');
                getline(ss, levelStr, ',');
                getline(ss, sickStr, ',');
                getline(ss, paidStr, ',');
                getline(ss, baseStr, ',');
                getline(ss, salesStr, ',');
                getline(ss, rateStr);

                //处理最后一个字段
                trim(rateStr);

                auto emp = make_unique<Salesman>(id, name, gender, dept, stoi(levelStr),
                    stod(baseStr), stod(salesStr), stod(rateStr));
                emp->setSickHours(stoi(sickStr));
                emp->setPaidLeaveHours(stoi(paidStr));
                employees.push_back(move(emp));
            }
            else if (type == "SM") {
                string id, name, gender, dept, levelStr, sickStr, paidStr, salaryStr, salesStr, rateStr;
                getline(ss, id, ',');
                getline(ss, name, ',');
                getline(ss, gender, ',');
                getline(ss, dept, ',');
                getline(ss, levelStr, ',');
                getline(ss, sickStr, ',');
                getline(ss, paidStr, ',');
                getline(ss, salaryStr, ',');
                getline(ss, salesStr, ',');
                getline(ss, rateStr);

                // 去除换行符
                trim(salaryStr);
                trim(salesStr);
                trim(rateStr);
                auto emp = make_unique<SalesManager>(id, name, gender, dept, stoi(levelStr),
                    stod(salaryStr), stod(salesStr), stod(rateStr));
                emp->setSickHours(stoi(sickStr));
                emp->setPaidLeaveHours(stoi(paidStr));
                employees.push_back(move(emp));
            }
        }
        cout << "数据加载成功！" << endl;
    }

    void statistics() const {
        double totalSalary = 0;
        int totalSick = 0, totalPaid = 0;
        for (const auto& emp : employees) {
            totalSalary += emp->calculateSalary();
            totalSick += emp->getSickHours();
            totalPaid += emp->getPaidLeaveHours();
        }
        cout << "==== 统计信息 ====" << endl;
        cout << "总薪资支出: " << totalSalary << endl;
        ; cout << "总病假小时: " << totalSick << endl;
        cout << "总带薪假小时: " << totalPaid << endl;
    }
};

void displayMenu() {
    cout << "\n==== 考勤管理系统 ===="
        << "\n 1. 添加员工"
        << "\n 2. 查询员工"
        << "\n 3. 显示所有记录"
        << "\n 4. 编辑休假记录"
        << "\n 5. 删除员工"
        << "\n 6. 统计信息"
        << "\n 7. 保存数据"
        << "\n 8. 加载数据"
        << "\n 0. 退出\n请选择操作: ";
    ;
}

int main() {
    AttendanceSystem system;
    int choice;

    do {
        displayMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            string type, id, name, gender, dept;
            int level;
            cout << "输入员工类型（M: 经理，T: 技术员，S: 销售员，SM: 销售经理）: ";
            getline(cin, type);
            cout << "输入员工ID: ";
            getline(cin, id);
            if (system.findEmployee(id) != nullptr) {
                cout << "员工ID已存在！" << endl;
                break;
            }
            cout << "姓名: ";
            getline(cin, name);
            cout << "性别: ";
            ; getline(cin, gender);
            cout << "部门: ";
            getline(cin, dept);
            cout << "级别（整数）: ";
            cin >> level;
            cin.ignore();

            if (type == "M" || type == "m") {
                double salary;
                cout << "固定工资: ";
                cin >> salary;
                cin.ignore();
                system.addEmployee(make_unique<Manager>(id, name, gender, dept, level, salary));
            }
            else if (type == "T" || type == "t") {
                double rate;
                int hours;
                cout << "时薪: ";
                cin >> rate;
                cout << "本月工作小时: ";
                cin >> hours;
                cin.ignore();
                system.addEmployee(make_unique<Technician>(id, name, gender, dept, level, rate, hours));
            }
            else if (type == "S" || type == "s") {
                double base, sales, rate;
                cout << "底薪: ";
                cin >> base;
                cout << "本月销售额: ";
                cin >> sales;
                cout << "提成比例(0-1): ";
                cin >> rate;
                cin.ignore();
                system.addEmployee(make_unique<Salesman>(id, name, gender, dept, level, base, sales, rate));
            }
            else if (type == "SM" || type == "sm") {
                double salary, sales, rate;
                cout << "固定工资: ";
                cin >> salary;
                cout << "团队销售额: ";
                cin >> sales;
                cout << "团队提成比例(0-1): ";
                cin >> rate;
                cin.ignore();
                system.addEmployee(make_unique<SalesManager>(id, name, gender, dept, level, salary, sales, rate));
            }
            else {
                cout << "无效的员工类型！" << endl;
            }
            break;
        }
        case 2: {
            string id;
            cout << "输入要查询的员工ID: ";
            getline(cin, id);
            Employee* emp = system.findEmployee(id);
            if (emp) {
                emp->display();
                emp->getLeaveRecord()->display();
            }
            else {
                cout << "未找到该员工！" << endl;
            }
            break;
        }
        case 3:
            system.displayAll();
            break;
        case 4: {
            string id;
            cout << "输入要修改的员工ID: ";
            getline(cin, id);
            Employee* emp = system.findEmployee(id);
            if (!emp) {
                cout << "员工不存在！" << endl;
                break;
            }

            int newSick, newPaid;
            cout << "当前病假小时: " << emp->getSickHours() << " 输入新值（0-16）: ";
            cin >> newSick;
            cout << "当前带薪假小时: " << emp->getPaidLeaveHours() << " 输入新值（0-24）: ";
            cin >> newPaid;
            cin.ignore();

            if (newSick >= 0 && newSick <= 16 && newPaid >= 0 && newPaid <= 24) {
                emp->setSickHours(newSick);
                emp->setPaidLeaveHours(newPaid);
                cout << "修改成功！" << endl;
            }
            else {
                cout << "输入数据无效！" << endl;
            }
            break;
        }
        case 5: {
            string id;
            cout << "输入要删除的员工ID: ";
            getline(cin, id);
            if (system.deleteEmployee(id)) {
                cout << "员工已删除！" << endl;
            }
            else {
                cout << "员工不存在！" << endl;
            }
            break;
        }
        case 6:
            system.statistics();
            break;
        case 7:
            system.saveToFile("data.csv");
            break;
        case 8:
            system.loadFromFile("data.csv");
            break;
        case 0:
            cout << "感谢使用！" << endl;
            break;
        default:
            cout << "无效操作！" << endl;
            ;
        }
    } while (choice != 0);

    return 0;
}