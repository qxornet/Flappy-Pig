#ifndef ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H
#define ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H

#include <android/log.h>
#include <sstream>

/*!
 * Используйте этот объект для вывода строк в logcat.
 * Обратите внимание, что для завершения и отправки строки необходимо использовать std::endl.
 *
 * Пример:
 *  aout << "Hello World" << std::endl;
 */
extern std::ostream aout;

/*!
 * Используйте этот класс для создания потока вывода, записывающего данные в logcat.
 * По умолчанию глобальный экземпляр определён как @a aout.
 */
class AndroidOut: public std::stringbuf {
public:
    /*!
     * Создаёт новый поток вывода для logcat.
     * @param kLogTag Тег журнала (log tag), который будет использоваться при выводе.
     */
    inline AndroidOut(const char* kLogTag) : logTag_(kLogTag){}

protected:
    virtual int sync() override {
        __android_log_print(ANDROID_LOG_DEBUG, logTag_, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* logTag_;
};

#endif //ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H