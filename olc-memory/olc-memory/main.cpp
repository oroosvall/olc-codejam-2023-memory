#include "olcPixelGameEngine.h"
#include "olc_PGEX_SplashScreen.h"

const int width = 512;
const int height = 512;

std::string formatNum(float f)
{
    std::string num_text = std::to_string(f);
    std::string rounded = num_text.substr(0, num_text.find(".") + 3);
    return rounded;
}

class ProgressBar
{
public:

    ProgressBar(const olc::vi2d& pos, const olc::vi2d& size, float min, float max)
        : mPos(pos)
        , mSize(size)
        , mMin(min)
        , mMax(max)
        , mValue(min)
    {
    }

    float getValue() const
    {
        return mValue;
    }

    void setValue(float value)
    {
        mValue = value;
    }

    void setMax(float value)
    {
        mMax = value;
    }

    float getMax() const
    {
        return mMax;
    }

    void draw(olc::PixelGameEngine* pge)
    {
        float percent = (mValue - mMin) / (mMax - mMin);

        olc::vi2d fillSize = { int(float(mSize.x) * percent), mSize.y };
        pge->FillRectDecal(mPos, mSize, mBackground);
        pge->FillRectDecal(mPos, fillSize, mFill);
        pge->DrawRectDecal(mPos, mSize, mBorder);
    }

private:

    olc::vi2d mPos;
    olc::vi2d mSize;
    float mMin;
    float mMax;
    float mValue;

    olc::Pixel mBackground = { 183, 73, 0, 255 };
    olc::Pixel mBorder = { 0,0,0,255 };
    olc::Pixel mFill = { 255, 159, 0, 255 };
};

class ShapeBar
{
public:
    ShapeBar(const olc::vi2d& center, const olc::vi2d& size)
        : mCenter(center)
        , mSize(size)
    {
    }

    void clear()
    {
        mList.clear();
    }

    void add(olc::Decal* nr)
    {
        mList.push_back(nr);
    }

    void select(int nr)
    {
        if (nr >= (int)mList.size())
        {
            nr = 0;
        }
        else if (nr < 0)
        {
            nr = (int)mList.size() - 1;
        }
        mSelected = nr;
    }

    int getSelectedIndex() const
    {
        return mSelected;
    }

    olc::Decal* getSelectedDecal() const
    {
        return mList[mSelected];
    }

    void draw(olc::PixelGameEngine* pge)
    {
        olc::vi2d later = { -1,-1 };
        olc::vi2d start = mCenter - ((mSize / 2) * int(mList.size()));
        start.y = mCenter.y;
        for (int i = 0; i < mList.size(); i++)
        {
            olc::Pixel p = mBorder;
            if (i == mSelected)
            {
                later = start;
            }
            else
            {
                pge->DrawDecal(start, mList[i]);
                pge->DrawRectDecal(start, mSize, mBorder);
            }
            start.x += mSize.x;
        }

        if (mSelected != -1)
        {
            pge->DrawDecal(later, mList[mSelected]);
            pge->DrawRectDecal(later, mSize, mSelectedBorder);
        }
    }

private:
    olc::vi2d mCenter;
    olc::vi2d mSize;

    olc::Pixel mBackground = { 183, 73, 0, 255 };
    olc::Pixel mBorder = { 0,0,0,255 };
    olc::Pixel mSelectedBorder = { 255, 255, 255, 255 };
    olc::Pixel mFill = { 255, 159, 0, 255 };

    std::vector<olc::Decal*> mList;
    int mSelected = 0;

};

class PlayGrid
{
public:
    PlayGrid(const olc::vi2d& center, const olc::vi2d& size)
        : mCenter(center)
        , mSize(size)
        , mGridSize()
        , mSolution()
        , mData()
        , mHoverIndex({ -1, -1 })
    {
    }

    void setTile(olc::Decal* tile)
    {
        mTile = tile;
    }

    void loadData(const olc::vi2d& size, const std::vector<olc::Decal*>& data)
    {
        mGridSize = size;
        mSolution = data;

        int s = size.x * size.y;
        mData.clear();
        mData.resize(s);
    }

    void hover(const olc::vi2d& pos)
    {
        mHoverIndex = pos;
    }

    void place(const olc::vi2d& pos, olc::Decal* value)
    {
        int idx = pos.y * mGridSize.x + pos.x;
        if (idx >= 0 && idx < mData.size())
        {
            mData[idx] = value;
        }
    }

    olc::vi2d transofrormCursor(const olc::vi2d& cursor)
    {
        olc::vi2d out{ -1, -1 };
        olc::vi2d start = mCenter - ((mSize / 2) * mGridSize);
        olc::vi2d end = mCenter + ((mSize / 2) * mGridSize);

        if (cursor.x > start.x && cursor.x < end.x
            && cursor.y > start.y && cursor.y < end.y)
        {
            olc::vi2d res = cursor - start;
            out = res / mSize;
        }

        return out;
    }

    void drawSolution(olc::PixelGameEngine* pge)
    {
        olc::vi2d later;
        olc::vi2d start = mCenter - ((mSize / 2) * mGridSize);

        for (int x = 0; x < mGridSize.x; x++)
        {
            int y = start.y;
            for (int y = 0; y < mGridSize.y; y++)
            {
                olc::Decal* d = mSolution[y * mGridSize.x + x];
                pge->DrawDecal(start, mTile);
                if (d)
                {
                    pge->DrawDecal(start, d);
                }
                pge->DrawRectDecal(start, mSize, mBorder);
                start.y += mSize.y;
            }
            start.x += mSize.x;
            start.y = y;
        }
    }

    void draw(olc::PixelGameEngine* pge)
    {
        olc::vi2d later;
        olc::vi2d start = mCenter - ((mSize / 2) * mGridSize);

        olc::vi2d hoverPos = { -1, -1 };

        for (int x = 0; x < mGridSize.x; x++)
        {
            int y = start.y;
            for (int y = 0; y < mGridSize.y; y++)
            {
                pge->DrawDecal(start, mTile);
                if (x == mHoverIndex.x && y == mHoverIndex.y)
                {
                    hoverPos = start;
                }
                else
                {
                    olc::Decal* d = mData[y * mGridSize.y + x];
                    if (d)
                    {
                        pge->DrawDecal(start, d);
                    }
                    pge->DrawRectDecal(start, mSize, mBorder);
                }
                start.y += mSize.y;
            }
            start.x += mSize.x;
            start.y = y;
        }

        if (hoverPos.x != -1 && hoverPos.y != -1)
        {
            olc::Decal* d = mData[mHoverIndex.y * mGridSize.y + mHoverIndex.x];
            if (d)
            {
                pge->DrawDecal(hoverPos, d);
            }
            pge->DrawRectDecal(hoverPos, mSize, mHoverBorder);
        }
    }

    int getScrore()
    {
        int score = 0;
        for (int i = 0; i < mData.size(); i++)
        {
            if (mData[i] == mSolution[i] && mSolution[i] != nullptr)
            {
                score++;
            }
        }

        return score;
    }

    int getMaxScore()
    {
        int score = 0;
        for (int i = 0; i < mSolution.size(); i++)
        {
            if (mSolution[i])
            {
                score++;
            }
        }

        return score;
    }

private:
    olc::vi2d mCenter;
    olc::vi2d mSize;

    olc::vi2d mGridSize;
    std::vector<olc::Decal*> mSolution;
    std::vector<olc::Decal*> mData;

    olc::vi2d mHoverIndex;
    olc::Decal* mTile = nullptr;
    olc::Pixel mBorder = { 0,0,0,255 };
    olc::Pixel mHoverBorder = { 255,255,255,255 };

};

struct LevelData
{
    float mTime;
    std::vector<olc::Decal*> mDecals;
    int mSizeX;
    int mSizeY;
    std::vector<olc::Decal*> mLevelGrid;
};

class LevelLoader
{
public:
    LevelLoader(std::string mainFile)
    {
        std::ifstream inFile(mainFile);
        if (inFile.is_open())
        {
            std::string line;
            while (!inFile.eof())
            {
                std::getline(inFile, line);
                // Remove crappy return
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
                mLevelFiles.push_back(line);
            }
        }
    }

    void loadDecals()
    {
        mDecals.resize(4);
        mDecals[0].Load("data/decals/StarShape.png");
        mDecals[1].Load("data/decals/RombShape.png");
        mDecals[2].Load("data/decals/FourLines.png");
        mDecals[3].Load("data/decals/Triangle.png");
    }

    int getNumLevels() const
    {
        return (int)mLevelFiles.size();
    }

    void addGridDecal(LevelData& l, int index)
    {
        if (index == -1)
        {
            l.mLevelGrid.push_back(nullptr);
        }
        else
        {
            l.mLevelGrid.push_back(mDecals[index].Decal());
        }
    }

    LevelData loadLevel(int index)
    {
        LevelData l{};
        std::ifstream inFile(mLevelFiles[index]);
        if (inFile.is_open())
        {
            std::string timerStr;
            std::getline(inFile, timerStr);
            l.mTime = std::atof(timerStr.c_str());

            std::string decalList;
            std::getline(inFile, decalList);
            size_t pos = 0;
            while ((pos = decalList.find(",")) != std::string::npos)
            {
                std::string token = decalList.substr(0, pos);
                l.mDecals.push_back(mDecals[std::atoi(token.c_str())].Decal());
                decalList.erase(0, pos + 1);
            }
            l.mDecals.push_back(mDecals[std::atoi(decalList.c_str())].Decal());

            std::string sizeStr;
            std::getline(inFile, sizeStr);
            pos = 0;
            if ((pos = sizeStr.find(",")) != std::string::npos)
            {
                std::string token = sizeStr.substr(0, pos);
                l.mSizeX = std::atoi(token.c_str());
                sizeStr.erase(0, pos + 1);
            }
            l.mSizeY = std::atoi(sizeStr.c_str());

            for (size_t i = 0; i < l.mSizeY; i++)
            {
                std::string gridStr;
                std::getline(inFile, gridStr);
                pos = 0;
                while ((pos = gridStr.find(",")) != std::string::npos)
                {
                    std::string token = gridStr.substr(0, pos);
                    addGridDecal(l, std::atoi(token.c_str()));
                    gridStr.erase(0, pos + 1);
                }
                addGridDecal(l, std::atoi(gridStr.c_str()));
            }
        }

        return l;
    }

private:
    std::vector<std::string> mLevelFiles;
    std::vector<olc::Renderable> mDecals;

};

enum class GameState
{
    FadeIn,
    Intro,
    Tutorial,
    Load,
    WaitInput,
    Present,
    Play,
    Score,
    End
};

class Memory : public olc::PixelGameEngine
{
public:
    Memory()
    {
        // Name your application
        sAppName = "Memory";
    }

    olc::SplashScreen mSplashScreen;
    LevelLoader mLevelLoader = { "data/levels.txt" };

    olc::Pixel mBackgroundColor = { 255, 106, 0, 255 };
    ProgressBar mTimerBar = { {100,10}, {width - 200, 10}, 0.0f, 100.0f };
    ShapeBar mShapeBar = { {width / 2, height - 50}, {32, 32} };
    PlayGrid mPlayGrid = { {width / 2, height / 2}, {32, 32} };

    GameState mGameState = GameState::FadeIn;

    olc::Renderable mGridTile;

    float mScrollCoolDown = 0.0f;
    const float mScrollTime = 0.05f;

    int mLevelIndex = 0;
    int mScore = 0;
    int mMaxScore = 0;

    std::vector<olc::Renderable> mShapes;
    olc::Renderable mIntro;
    olc::Renderable mBackground;
    olc::Renderable mDemoShape;

    LevelData mLevelData;

    float mFade = 0.0;

    olc::Decal* mActiveBg;

public:
    bool OnUserCreate() override
    {
        mIntro.Load("data/decals/Intro.png");
        mBackground.Load("data/decals/Background.png");

        olc::vi2d size = mIntro.Sprite()->Size();
        olc::Pixel* data = mIntro.Sprite()->GetData();
        for (int i = 0; i < size.x * size.y; i++)
        {
            data[i].a = 0;
        }
        mIntro.Decal()->Update();

        mDemoShape.Load("data/decals/StarShape.png");
        mLevelLoader.loadDecals();
        mGridTile.Load("data/decals/GridTile.png");
        mPlayGrid.setTile(mGridTile.Decal());

        std::vector<olc::Decal*> dec;
        dec.resize(4 * 4, nullptr);
        mPlayGrid.loadData({ 4,4 }, dec);
        mShapeBar.add(mDemoShape.Decal());

        mTimerBar.setValue(50.0f);
        mActiveBg = mIntro.Decal();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        //FillRectDecal({ 0,0 }, { width, height }, mBackgroundColor);
        DrawDecal({ 0,0 }, mActiveBg);

        if (mGameState != GameState::End && mGameState != GameState::Intro && mGameState != GameState::Tutorial && mGameState != GameState::FadeIn)
        {
            std::string levelText = "Level " + std::to_string(mLevelIndex + 1) + "/" + std::to_string(mLevelLoader.getNumLevels());
            DrawStringPropDecal({ 10,10, }, levelText);

            std::string scoreText = "Score: " + std::to_string(mScore);
            DrawStringPropDecal({ 10,25, }, scoreText);
        }

        switch (mGameState)
        {
        case GameState::FadeIn:
        {
            mFade += fElapsedTime;
            if (mFade >= 0.5f)
            {
                mFade = 0.5f;
                mGameState = GameState::Intro;
            }
            olc::vi2d size = mIntro.Sprite()->Size();
            olc::Pixel* data = mIntro.Sprite()->GetData();
            for (int i = 0; i < size.x * size.y; i++)
            {
                data[i].a = int(2.0f * mFade * 255.0f);
            }
            mIntro.Decal()->Update();
        }
        break;
        case GameState::Intro:
        {
            std::string txt = "Press Space to continue to the How to play.";
            auto size = GetTextSizeProp(txt);
            olc::vf2d pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos, txt);
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mActiveBg = mBackground.Decal();
                mGameState = GameState::Tutorial;
            }
        }
        break;
        case GameState::Tutorial:
        {
            mTimerBar.draw(this);
            mPlayGrid.drawSolution(this);
            mShapeBar.draw(this);

            std::string timeText = "There will be a timer bar during the presentation,\nincreasing untill full.\nThen the guessing starts.";
            DrawStringPropDecal({ 100, 25 }, timeText);

            std::string boardText = "The game board is where you will have to place shapes.\nA white highlight will show hovered tile.\nUse left click to place, and right click to clear";
            DrawStringPropDecal({ 100, (height / 2) + 32 * 2 }, boardText);

            std::string shapeText = "Select a shape with scroll wheel.\nA white outline will highlight the selection.";
            DrawStringPropDecal({ 100, height - 75 }, shapeText);

            std::string continueText = "Press Space to Begin!";
            auto size = GetTextSizeProp(continueText);
            olc::vf2d pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos, continueText);

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mGameState = GameState::Load;
            }
        }
        break;
        case GameState::Load:
        {
            mLevelData = mLevelLoader.loadLevel(mLevelIndex);
            mGameState = GameState::WaitInput;
        }
        break;
        case GameState::WaitInput:
        {
            std::string txt = "You will have " + formatNum(mLevelData.mTime) + " s to remember...";
            auto size = GetTextSizeProp(txt);
            olc::vf2d pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos, txt);
            std::string txt2 = "Press Space to reveal";
            size = GetTextSizeProp(txt2);
            pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos + olc::vf2d{ 0.0f,15.0f }, txt2);

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mShapeBar.clear();
                for (auto dec : mLevelData.mDecals)
                {
                    mShapeBar.add(dec);
                }
                mShapeBar.select(0);
                mPlayGrid.loadData({ mLevelData.mSizeX, mLevelData.mSizeY }, mLevelData.mLevelGrid);

                mGameState = GameState::Present;
                mTimerBar.setValue(0.0f);
                mTimerBar.setMax(mLevelData.mTime);
            }
        }
        break;
        case GameState::Present:
        {
            float v = mTimerBar.getValue();
            float toReach = mTimerBar.getMax();

            if (v > toReach)
            {
                mGameState = GameState::Play;
            }
            mTimerBar.setValue(v + fElapsedTime);
            mTimerBar.draw(this);
            mPlayGrid.drawSolution(this);
        }
        break;
        case GameState::Play:
        {
            olc::vi2d mousePos = GetMousePos();
            olc::vi2d pos = mPlayGrid.transofrormCursor(mousePos);

            int scrollDelta = GetMouseWheel();
            if (scrollDelta != 0 && mScrollCoolDown <= 0.0f)
            {
                if (scrollDelta > 0)
                {
                    int selectedIdx = mShapeBar.getSelectedIndex();
                    mShapeBar.select(selectedIdx + 1);
                }
                else
                {
                    int selectedIdx = mShapeBar.getSelectedIndex();
                    mShapeBar.select(selectedIdx - 1);
                }
                mScrollCoolDown = mScrollTime;
            }
            else if (mScrollCoolDown > 0.0)
            {
                mScrollCoolDown -= fElapsedTime;
            }

            mShapeBar.draw(this);
            mPlayGrid.hover(pos);
            if (GetMouse(olc::Mouse::LEFT).bPressed)
            {
                mPlayGrid.place(pos, mShapeBar.getSelectedDecal());
            }
            else if (GetMouse(olc::Mouse::RIGHT).bPressed)
            {
                mPlayGrid.place(pos, nullptr);
            }
            mPlayGrid.draw(this);

            std::string txt = "Press Space to submit";
            auto size = GetTextSizeProp(txt);
            olc::vf2d textPos = { float((width / 2) - (size.x / 2)) , float((height - 75) - (size.y / 2)) };
            DrawStringPropDecal(textPos, txt);

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mGameState = GameState::Score;
            }
        }
        break;
        case GameState::Score:
        {
            int score = mPlayGrid.getScrore();
            int maxScore = mPlayGrid.getMaxScore();

            std::string scoreText = "You scored : " + std::to_string(score) + "/" + std::to_string(maxScore);
            auto size = GetTextSizeProp(scoreText);
            olc::vf2d textPos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(textPos, scoreText);

            std::string txt = "Press Space to continue";
            size = GetTextSizeProp(txt);
            textPos = { float((width / 2) - (size.x / 2)) , float((height - 75) - (size.y / 2)) };
            DrawStringPropDecal(textPos, txt);
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mScore += score;
                mMaxScore += maxScore;
                mGameState = GameState::Load;

                mLevelIndex++;
                if (mLevelIndex == mLevelLoader.getNumLevels())
                {
                    mGameState = GameState::End;
                }
            }
        }
        break;
        case GameState::End:
        {
            std::string txt = "Thank you for playing";
            auto size = GetTextSizeProp(txt);
            olc::vf2d pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos, txt);

            std::string scoreText = "Your total score was : " + std::to_string(mScore) + "/" + std::to_string(mMaxScore);
            size = GetTextSizeProp(scoreText);
            olc::vf2d textPos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos + olc::vf2d{ 0.0f, 15.0f }, scoreText);

        }
        break;
        default:
            break;
        }
        return true;
    }
};

int main()
{
    Memory app;
    if (app.Construct(width, height, 2, 2, false, true))
        app.Start();
    return 0;
}