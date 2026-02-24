#include "Manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>

using namespace std;

namespace {
    // command.txt
    constexpr size_t CMD_FILE_MAX_BYTES = 10 * 1024 * 1024; // 10MB
    constexpr size_t CMD_LINE_MAX_BYTES = 2048;             // per line
    constexpr size_t CMD_TOKEN_MAX      = 8;                // tail token count
    constexpr size_t CMD_TOKEN_MAX_LEN  = 64;               // token length
    constexpr size_t CMD_TAIL_MAX_BYTES = 512;              // tail bytes (for cmd mirror)

    // employee.txt
    constexpr size_t EMP_FILE_MAX_BYTES = 20 * 1024 * 1024; // 20MB
    constexpr size_t EMP_LINE_MAX_BYTES = 256;              // one record line
    constexpr size_t EMP_RECORDS_MAX    = 500000;           // safety cap

    inline bool isWS(unsigned char c) {
        return c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\f' || c=='\v';
    }
    inline bool isPrintOrWS(unsigned char c) {
        return (c >= 32 && c <= 126) || isWS(c);
    }
    inline bool allPrintOrWS(const string& s) {
        for (unsigned char c : s) if (c == 0 || !isPrintOrWS(c)) return false;
        return true;
    }

    // map op -> error code (spec)
    inline int errCode(const string& op) {
        if (op=="LOAD")      return 100;
        if (op=="ADD_BP")    return 200;
        if (op=="SEARCH_BP") return 300;
        if (op=="PRINT_BP")  return 400;
        if (op=="ADD_ST")    return 500;
        if (op=="PRINT_ST")  return 600;
        if (op=="DELETE")    return 700;
        return 800; // unknown
    }
}

// trim spaces/tabs/CR
static inline void trimL(string& s) {
    size_t i=0; while (i<s.size() && isWS((unsigned char)s[i])) ++i;
    if (i) s.erase(0,i);
}
static inline void trimR(string& s) {
    size_t i=s.size(); while (i && isWS((unsigned char)s[i-1])) --i;
    s.resize(i);
}
static inline void trimAll(string& s) { trimR(s); trimL(s); }

// full-line comment
static inline bool isComment(const string& s) {
    if (s.empty()) return false;
    return (s[0]=='#' || s[0]==';' || (s.size()>=2 && s[0]=='/' && s[1]=='/'));
}

// cut inline comment 
static inline void cutInlineComment(string& s) {
    size_t cut = string::npos;
    size_t a = s.find("//");
    size_t b = s.find('#');
    size_t c = s.find(';');
    if (a!=string::npos) cut=min(cut,a);
    if (b!=string::npos) cut=min(cut,b);
    if (c!=string::npos) cut=min(cut,c);
    if (cut!=string::npos) s.erase(cut);
    trimAll(s);
}

// tokenize with caps (count/length). invalid -> empty
static inline vector<string> tokenize(const string& s) {
    vector<string> out; out.reserve(4);
    istringstream iss(s);
    string w;
    while (iss >> w) {
        if (w.size() > CMD_TOKEN_MAX_LEN) { out.clear(); return out; }
        out.push_back(w);
        if (out.size() > CMD_TOKEN_MAX)   { out.clear(); return out; }
    }
    return out;
}

// name: lowercase only, <=10
static inline bool isLowerName(const string& s) {
    if (s.empty() || s.size() > 10) return false;
    return all_of(s.begin(), s.end(), [](unsigned char c){ return c>='a' && c<='z'; });
}

// strict unsigned int (base-10 only, no sign/space), overflow-safe
static inline bool parseIntStrict(const string& s, int& dst) {
    if (s.empty()) return false;
    if (s[0]=='+' || s[0]=='-') return false;
    long long v=0;
    for (unsigned char ch : s) {
        if (ch<'0' || ch>'9') return false;
        v = v*10 + (ch-'0');
        if (v > std::numeric_limits<int>::max()) return false;
    }
    dst = (int)v; return true;
}

// dept whitelist
static inline bool validDept(int d) {
    switch (d) {
        case 100: case 200: case 300: case 400:
        case 500: case 600: case 700: case 800: return true;
        default: return false;
    }
}

// command token: A-Z and '_' only (<=16)
static inline bool isUpperCmd(const string& s) {
    if (s.empty() || s.size() > 16) return false;
    for (unsigned char c : s) if (!(c=='_' || (c>='A' && c<='Z'))) return false;
    return true;
}

// safe copy for legacy cmd C-string
static inline void setCmdMirror(char*& cmd, const string& tail) {
    size_t n = min(tail.size(), (size_t)CMD_TAIL_MAX_BYTES);
    if (cmd) { delete[] cmd; cmd=nullptr; }
    cmd = new (std::nothrow) char[n+1];
    if (!cmd) return;
    if (n) std::memcpy(cmd, tail.data(), n);
    cmd[n] = '\0';
}

void Manager::run(const char* command) {
    // open log.txt (truncate)
    flog.open("log.txt", ios::out | ios::trunc);
    if (!flog.is_open()) { cerr << "[fatal] open log.txt failed\n"; return; }

    // build DS
    if (!bptree) bptree = new BpTree(&flog, bpOrder);
    if (!stree)  stree  = new SelectionTree(&flog);

    // open command file (binary -> size check)
    fin.open(command, ios::in | ios::binary);
    if (!fin.is_open()) { cerr << "[fatal] cannot open command file\n"; return; }

    // file size limit
    fin.seekg(0, ios::end);
    std::streamoff fs = fin.tellg();
    if (fs < 0 || (unsigned long long)fs > CMD_FILE_MAX_BYTES) { cerr << "[fatal] command too large\n"; return; }
    fin.seekg(0, ios::beg);

    string line;
    while (std::getline(fin, line)) {
        // line guard
        if (line.size() > CMD_LINE_MAX_BYTES || !allPrintOrWS(line)) { printErrorCode(800); continue; }

        trimAll(line);
        cutInlineComment(line);
        if (line.empty() || isComment(line)) continue;

        // split op / tail
        string op, tail;
        {
            istringstream iss(line);
            iss >> op;
            std::getline(iss, tail);
            trimAll(tail);
        }

        // op must be UPPER (spec)  :contentReference[oaicite:1]{index=1}
        if (!isUpperCmd(op)) { printErrorCode(800); continue; }

        // tail guard
        if (tail.size() > CMD_TAIL_MAX_BYTES || !allPrintOrWS(tail)) { printErrorCode(errCode(op)); continue; }

        lastCmdTail = tail;          // keep raw tail
        setCmdMirror(cmd, tail);     // legacy mirror

        // dispatch
        if (op == "LOAD") {
            if (!lastCmdTail.empty()) { printErrorCode(100); continue; }
            LOAD();
        }
        else if (op == "ADD_BP") {
            ADD_BP();
        }
        else if (op == "SEARCH_BP") {
            auto t = tokenize(tail);
            if (t.empty()) { printErrorCode(300); continue; }
            if (t.size()==1)      SEARCH_BP_NAME(t[0]);
            else if (t.size()==2) SEARCH_BP_RANGE(t[0], t[1]);
            else                  printErrorCode(300);
        }
        else if (op == "PRINT_BP") {
            if (!lastCmdTail.empty()) printErrorCode(400);
            else                      PRINT_BP();
        }
        else if (op == "ADD_ST") {
            auto t = tokenize(tail);
            if (t.size()!=2) { printErrorCode(500); continue; }

            if (t[0] == "dept_no") {
                int d;
                if (!parseIntStrict(t[1], d) || !validDept(d)) { printErrorCode(500); continue; }
                ADD_ST_DEPTNO(d);
            } else if (t[0] == "name") {
                if (t[1].size() > CMD_TOKEN_MAX_LEN) { printErrorCode(500); continue; }
                ADD_ST_NAME(t[1]);
            } else {
                printErrorCode(500);
            }
        }
        else if (op == "PRINT_ST") {
            auto t = tokenize(tail);
            if (t.size()!=1) { printErrorCode(600); continue; }
            int d;
            if (!parseIntStrict(t[0], d) || !validDept(d)) { printErrorCode(600); continue; }
            lastCmdTail = t[0];
            PRINT_ST();
        }
        else if (op == "DELETE") {
            if (!lastCmdTail.empty()) printErrorCode(700);
            else                      DELETE();
        }
        else if (op == "EXIT") {
            if (!lastCmdTail.empty()) { printErrorCode(800); continue; }
            printSuccessCode("EXIT");
            break;
        }
        else {
            printErrorCode(800);
        }
    }

    // legacy cmd buffer tidy
    if (cmd) { delete[] cmd; cmd=nullptr; }
}


void Manager::LOAD() {
    if (!dsEmpty) { printErrorCode(100); return; }

    ifstream ein("employee.txt", ios::in | ios::binary);
    if (!ein.is_open()) { printErrorCode(100); return; }

    // size guard
    ein.seekg(0, ios::end);
    std::streamoff sz = ein.tellg();
    if (sz < 0 || (unsigned long long)sz > EMP_FILE_MAX_BYTES) { printErrorCode(100); return; }
    ein.seekg(0, ios::beg);

    string raw;
    int cnt = 0;

    while (std::getline(ein, raw)) {
        if (raw.size() > EMP_LINE_MAX_BYTES || !allPrintOrWS(raw)) continue;
        trimAll(raw);
        cutInlineComment(raw);
        if (raw.empty() || isComment(raw)) continue;

        auto tok = tokenize(raw);
        if (tok.size()!=4) continue;  // spec: ignore malformed line

        const string& name = tok[0];
        int dept_no, id, income;

        if (!isLowerName(name)) continue;
        if (!parseIntStrict(tok[1], dept_no) || !validDept(dept_no)) continue;
        if (!parseIntStrict(tok[2], id)) continue;
        if (!parseIntStrict(tok[3], income)) continue;

        auto* rec = new (std::nothrow) EmployeeData();
        if (!rec) { printErrorCode(100); return; }
        rec->setData(name, dept_no, id, income);
        bptree->Insert(rec);
        ++cnt;

        if (cnt > (int)EMP_RECORDS_MAX) { printErrorCode(100); return; }
    }

    if (cnt > 0) dsEmpty = false;
    printSuccessCode("LOAD");
}

void Manager::ADD_BP() {
    auto t = tokenize(lastCmdTail);
    if (t.size()!=4) { printErrorCode(200); return; }

    const string& name = t[0];
    int dept_no, id, income;
    // validate all fields :contentReference[oaicite:2]{index=2}
    if (!isLowerName(name)) { printErrorCode(200); return; }
    if (!parseIntStrict(t[1], dept_no) || !validDept(dept_no)) { printErrorCode(200); return; }
    if (!parseIntStrict(t[2], id)) { printErrorCode(200); return; }
    if (!parseIntStrict(t[3], income)) { printErrorCode(200); return; }
    
    auto* rec = new (std::nothrow) EmployeeData();
    if (!rec) { printErrorCode(200); return; }
    rec->setData(name, dept_no, id, income);
    bptree->Insert(rec);   // duplicate name -> update income inside B+Tree
    dsEmpty = false;

    flog << "========ADD_BP========\n";
    flog << name << "/" << dept_no << "/" << id << "/" << income << "\n";
    flog << "=====================\n\n";
}
// Search by name
void Manager::SEARCH_BP_NAME(string name) {
    if (name.empty() || !isLowerName(name)) { printErrorCode(300); return; }
    if (dsEmpty) { printErrorCode(300); return; }
    (void)bptree->Search(name, flog);
}
// Search by range
void Manager::SEARCH_BP_RANGE(string start, string end) {
    if (start.empty() || end.empty() || !isLowerName(start) || !isLowerName(end)) { printErrorCode(300); return; }
    if (start > end) { printErrorCode(300); return; }
    if (dsEmpty) { printErrorCode(300); return; }
    (void)bptree->Search(start, end, flog);
}
// Print all B+Tree records
void Manager::PRINT_BP() {
    if (dsEmpty) { printErrorCode(400); return; }
    (void)bptree->Print(flog);
}
// Add SelectionTree by dept_no
void Manager::ADD_ST_DEPTNO(int dept_no) {
    if (dsEmpty) { printErrorCode(500); return; }
    bool ok = stree->AddByDeptFromBpTree(bptree, dept_no);
    if (!ok) { printErrorCode(500); return; }
    printSuccessCode("ADD_ST");
}
// Add SelectionTree by name
void Manager::ADD_ST_NAME(string name) {
    if (name.empty() || !isLowerName(name)) { printErrorCode(500); return; }
    if (dsEmpty) { printErrorCode(500); return; }
    bool ok = stree->AddByNameFromBpTree(bptree, name);
    if (!ok) { printErrorCode(500); return; }
    printSuccessCode("ADD_ST");
}
// Print SelectionTree by dept_no 
void Manager::PRINT_ST() {
    auto t = tokenize(lastCmdTail);
    if (t.size()!=1) { printErrorCode(600); return; }
    int dept_no;
    if (!parseIntStrict(t[0], dept_no) || !validDept(dept_no)) { printErrorCode(600); return; }
    (void)stree->PrintDept(flog, dept_no);
}
// Delete top of SelectionTree
void Manager::DELETE() {
    bool ok = stree->DeleteTop();
    if (!ok) { printErrorCode(700); return; }
    printSuccessCode("DELETE");
}

void Manager::printErrorCode(int n) {
    flog << "========ERROR========\n";
    flog << n << "\n";
    flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success) {
    flog << "========" << success << "========\n";
    flog << "Success" << "\n";
    flog << "====================\n\n";
}
