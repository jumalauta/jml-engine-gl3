#ifndef ENGINE_UI_MENU_MENU_H_
#define ENGINE_UI_MENU_MENU_H_

class Menu {
public:
    static Menu& getInstance();
    virtual ~Menu() {}
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual void render() = 0;
    virtual unsigned int getWidth() = 0;
    virtual unsigned int getHeight() = 0;
    bool isQuit();
    void setQuit(bool quit);
protected:
    Menu();
private:
    bool quit;
};

#endif /*ENGINE_UI_MENU_MENU_H_*/
