//
// Created by ken on 2023/7/7.
//

#ifndef MOXT_IDGEN_HPP
#define MOXT_IDGEN_HPP

#include <string>

class IDGen {
  public:
    static IDGen &getInstance() {
        static IDGen instance;
        return instance;
    }

    /*
     * 生成一个ID。
     */
    std::string id();

  private:
    IDGen() {}

    ~IDGen() = default;

    IDGen(const IDGen &) = delete;

    IDGen(IDGen &&) = delete;

    IDGen &operator=(const IDGen &) = delete;

    IDGen &operator=(IDGen &&) = delete;
};

#endif // MOXT_IDGEN_HPP
