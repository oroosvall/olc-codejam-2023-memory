#include "olcPixelGameEngine.h"

const int width = 512;
const int height = 512;

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

private:
    olc::vi2d mCenter;
    olc::vi2d mSize;

    olc::vi2d mGridSize;
    std::vector<olc::Decal*> mSolution;
    std::vector<olc::Decal*> mData;

    olc::vi2d mHoverIndex;
    olc::Pixel mBorder = { 0,0,0,255 };
    olc::Pixel mHoverBorder = { 255,255,255,255 };

};

enum class GameState
{
    WaitInput,
    Present,
    Play,
    Score
};

class Memory : public olc::PixelGameEngine
{
public:
    Memory()
    {
        // Name your application
        sAppName = "Memory";
    }

    olc::Pixel mBackgroundColor = { 255, 106, 0, 255 };
    ProgressBar mTimerBar = { {50,10}, {width - 100, 10}, 0.0f, 100.0f };
    ShapeBar mShapeBar = { {width / 2, height - 50}, {32, 32} };
    PlayGrid mPlayGrid = { {width / 2, height / 2}, {32, 32} };

    GameState mGameState = GameState::WaitInput;

    olc::Renderable mWhiteShape;
    olc::Renderable mRedShape;
    olc::Renderable mGreenShape;
    olc::Renderable mBlueShape;

    float mScrollCoolDown = 0.0f;
    const float mScrollTime = 0.05f;

public:
    bool OnUserCreate() override
    {
        mWhiteShape.Load("white.png");
        mRedShape.Load("red.png");
        mGreenShape.Load("green.png");
        mBlueShape.Load("blue.png");

        mTimerBar.setValue(50.0f);
        mShapeBar.add(mWhiteShape.Decal());
        mShapeBar.add(mRedShape.Decal());
        mShapeBar.add(mGreenShape.Decal());
        mShapeBar.add(mBlueShape.Decal());
        auto d = { mWhiteShape.Decal(), mRedShape.Decal(), mGreenShape.Decal(), mBlueShape.Decal() };
        mPlayGrid.loadData({ 2,2 }, d);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        FillRectDecal({ 0,0 }, { width, height }, mBackgroundColor);
        switch (mGameState)
        {
        case GameState::WaitInput:
        {
            std::string txt = "Press Space to reveal";
            auto size = GetTextSizeProp(txt);
            olc::vf2d pos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(pos, txt);
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mGameState = GameState::Present;
                mTimerBar.setValue(0.0f);
                mTimerBar.setMax(1.0f);
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

            std::string scoreText = "You scored : " + std::to_string(score);
            auto size = GetTextSizeProp(scoreText);
            olc::vf2d textPos = { float((width / 2) - (size.x / 2)) , float((height / 2) - (size.y / 2)) };
            DrawStringPropDecal(textPos, scoreText);

            std::string txt = "Press Space to continue";
            size = GetTextSizeProp(txt);
            textPos = { float((width / 2) - (size.x / 2)) , float((height - 75) - (size.y / 2)) };
            DrawStringPropDecal(textPos, txt);
            if (GetKey(olc::Key::SPACE).bPressed)
            {
                mGameState = GameState::WaitInput;
            }
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