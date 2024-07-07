// https://github.com/CedricGuillemet/ImGuizmo
// v1.91.3 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once
#include <stdint.h>
//#include "imgui.h"

struct ImRect;

namespace ImCurveEdit
{
   enum CurveType
   {
      CurveNone,
      CurveDiscrete,
      CurveLinear,
      CurveSmooth,
      CurveBezier,
   };

   struct EditPoint
   {
      int curveIndex;
      int pointIndex;
      bool operator <(const EditPoint& other) const
      {
         if (curveIndex < other.curveIndex)
            return true;
         if (curveIndex > other.curveIndex)
            return false;

         if (pointIndex < other.pointIndex)
            return true;
         return false;
      }
   };

   struct Delegate
   {
      bool focused = false;
      virtual size_t GetCurveCount() = 0;
      virtual bool IsVisible(size_t /*curveIndex*/) { return true; }
      virtual CurveType GetCurveType(size_t /*curveIndex*/) const { return CurveLinear; }
      virtual ImVec2& GetMin() = 0;
      virtual ImVec2& GetMax() = 0;
      virtual size_t GetPointCount(size_t curveIndex) = 0;
      virtual uint32_t GetCurveColor(size_t curveIndex) = 0;
      virtual ImVec2* GetPoints(size_t curveIndex) = 0;
      virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) = 0;
      virtual void AddPoint(size_t curveIndex, ImVec2 value) = 0;
      virtual unsigned int GetBackgroundColor() { return 0xFF202020; }
      // handle undo/redo thru this functions
      virtual void BeginEdit(int /*index*/) {}
      virtual void EndEdit() {}

      virtual ~Delegate() = default;
   };

   int Edit(Delegate& delegate, const ImVec2& size, unsigned int id, const ImRect* clippingRect = NULL, ImVector<EditPoint>* selectedPoints = NULL);
}

// https://github.com/CedricGuillemet/ImGuizmo
// v1.91.3 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//#include "ImCurveEdit.h"
//#include "imgui.h"
//#include "imgui_internal.h"
#include <stdint.h>
#include <set>
#include <vector>

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif
#if !defined(_MSC_VER) && !defined(__MINGW64_VERSION_MAJOR)
#define _malloca(x) alloca(x)
#define _freea(x)
#endif

namespace ImCurveEdit
{

#ifndef IMGUI_DEFINE_MATH_OPERATORS
   static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x + b.x, a.y + b.y);
   }

   static ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x - b.x, a.y - b.y);
   }

   static ImVec2 operator*(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x * b.x, a.y * b.y);
   }

   static ImVec2 operator/(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x / b.x, a.y / b.y);
   }

   static ImVec2 operator*(const ImVec2& a, const float b) {
      return ImVec2(a.x * b, a.y * b);
   }
#endif

   static float smoothstep(float edge0, float edge1, float x)
   {
      x = ImClamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
      return x * x * (3 - 2 * x);
   }

   static float distance(float x, float y, float x1, float y1, float x2, float y2)
   {
      float A = x - x1;
      float B = y - y1;
      float C = x2 - x1;
      float D = y2 - y1;

      float dot = A * C + B * D;
      float len_sq = C * C + D * D;
      float param = -1.f;
      if (len_sq > FLT_EPSILON)
         param = dot / len_sq;

      float xx, yy;

      if (param < 0.f) {
         xx = x1;
         yy = y1;
      }
      else if (param > 1.f) {
         xx = x2;
         yy = y2;
      }
      else {
         xx = x1 + param * C;
         yy = y1 + param * D;
      }

      float dx = x - xx;
      float dy = y - yy;
      return sqrtf(dx * dx + dy * dy);
   }

   static int DrawPoint(ImDrawList* draw_list, ImVec2 pos, const ImVec2 size, const ImVec2 offset, bool edited)
   {
      int ret = 0;
      ImGuiIO& io = ImGui::GetIO();

      static const ImVec2 localOffsets[4] = { ImVec2(1,0), ImVec2(0,1), ImVec2(-1,0), ImVec2(0,-1) };
      ImVec2 offsets[4];
      for (int i = 0; i < 4; i++)
      {
         offsets[i] = pos * size + localOffsets[i] * 4.5f + offset;
      }

      const ImVec2 center = pos * size + offset;
      const ImRect anchor(center - ImVec2(5, 5), center + ImVec2(5, 5));
      draw_list->AddConvexPolyFilled(offsets, 4, 0xFF000000);
      if (anchor.Contains(io.MousePos))
      {
         ret = 1;
         if (io.MouseDown[0])
            ret = 2;
      }
      if (edited)
         draw_list->AddPolyline(offsets, 4, 0xFFFFFFFF, true, 3.0f);
      else if (ret)
         draw_list->AddPolyline(offsets, 4, 0xFF80B0FF, true, 2.0f);
      else
         draw_list->AddPolyline(offsets, 4, 0xFF0080FF, true, 2.0f);

      return ret;
   }

   int Edit(Delegate& delegate, const ImVec2& size, unsigned int id, const ImRect* clippingRect, ImVector<EditPoint>* selectedPoints)
   {
      static bool selectingQuad = false;
      static ImVec2 quadSelection;
      static int overCurve = -1;
      static int movingCurve = -1;
      static bool scrollingV = false;
      static std::set<EditPoint> selection;
      static bool overSelectedPoint = false;

      int ret = 0;

      ImGuiIO& io = ImGui::GetIO();
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
      ImGui::PushStyleColor(ImGuiCol_Border, 0);
      ImGui::BeginChild(id, size, ImGuiChildFlags_FrameStyle);
      delegate.focused = ImGui::IsWindowFocused();
      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      if (clippingRect)
         draw_list->PushClipRect(clippingRect->Min, clippingRect->Max, true);

      const ImVec2 offset = ImGui::GetCursorScreenPos() + ImVec2(0.f, size.y);
      const ImVec2 ssize(size.x, -size.y);
      const ImRect container(offset + ImVec2(0.f, ssize.y), offset + ImVec2(ssize.x, 0.f));
      ImVec2& min = delegate.GetMin();
      ImVec2& max = delegate.GetMax();

      // handle zoom and VScroll
      if (container.Contains(io.MousePos))
      {
         if (fabsf(io.MouseWheel) > FLT_EPSILON)
         {
            const float r = (io.MousePos.y - offset.y) / ssize.y;
            float ratioY = ImLerp(min.y, max.y, r);
            auto scaleValue = [&](float v) {
               v -= ratioY;
               v *= (1.f - io.MouseWheel * 0.05f);
               v += ratioY;
               return v;
            };
            min.y = scaleValue(min.y);
            max.y = scaleValue(max.y);
         }
         if (!scrollingV && ImGui::IsMouseDown(2))
         {
            scrollingV = true;
         }
      }
      ImVec2 range = max - min + ImVec2(1.f, 0.f);  // +1 because of inclusive last frame

      const ImVec2 viewSize(size.x, -size.y);
      const ImVec2 sizeOfPixel = ImVec2(1.f, 1.f) / viewSize;
      const size_t curveCount = delegate.GetCurveCount();

      if (scrollingV)
      {
         float deltaH = io.MouseDelta.y * range.y * sizeOfPixel.y;
         min.y -= deltaH;
         max.y -= deltaH;
         if (!ImGui::IsMouseDown(2))
            scrollingV = false;
      }

      draw_list->AddRectFilled(offset, offset + ssize, delegate.GetBackgroundColor());

      auto pointToRange = [&](ImVec2 pt) { return (pt - min) / range; };
      auto rangeToPoint = [&](ImVec2 pt) { return (pt * range) + min; };

      draw_list->AddLine(ImVec2(-1.f, -min.y / range.y) * viewSize + offset, ImVec2(1.f, -min.y / range.y) * viewSize + offset, 0xFF000000, 1.5f);
      bool overCurveOrPoint = false;

      int localOverCurve = -1;
      // make sure highlighted curve is rendered last
      int* curvesIndex = (int*)_malloca(sizeof(int) * curveCount);
      for (size_t c = 0; c < curveCount; c++)
         curvesIndex[c] = int(c);
      int highLightedCurveIndex = -1;
      if (overCurve != -1 && curveCount)
      {
         ImSwap(curvesIndex[overCurve], curvesIndex[curveCount - 1]);
         highLightedCurveIndex = overCurve;
      }

      for (size_t cur = 0; cur < curveCount; cur++)
      {
         int c = curvesIndex[cur];
         if (!delegate.IsVisible(c))
            continue;
         const size_t ptCount = delegate.GetPointCount(c);
         if (ptCount < 1)
            continue;
         CurveType curveType = delegate.GetCurveType(c);
         if (curveType == CurveNone)
            continue;
         const ImVec2* pts = delegate.GetPoints(c);
         uint32_t curveColor = delegate.GetCurveColor(c);
         if ((c == highLightedCurveIndex && selection.empty() && !selectingQuad) || movingCurve == c)
            curveColor = 0xFFFFFFFF;

         for (size_t p = 0; p < ptCount - 1; p++)
         {
            const ImVec2 p1 = pointToRange(pts[p]);
            const ImVec2 p2 = pointToRange(pts[p + 1]);

            if (curveType == CurveSmooth || curveType == CurveLinear)
            {
               size_t subStepCount = (curveType == CurveSmooth) ? 20 : 2;
               float step = 1.f / float(subStepCount - 1);
               for (size_t substep = 0; substep < subStepCount - 1; substep++)
               {
                  float t = float(substep) * step;

                  const ImVec2 sp1 = ImLerp(p1, p2, t);
                  const ImVec2 sp2 = ImLerp(p1, p2, t + step);

                  const float rt1 = smoothstep(p1.x, p2.x, sp1.x);
                  const float rt2 = smoothstep(p1.x, p2.x, sp2.x);

                  const ImVec2 pos1 = ImVec2(sp1.x, ImLerp(p1.y, p2.y, rt1)) * viewSize + offset;
                  const ImVec2 pos2 = ImVec2(sp2.x, ImLerp(p1.y, p2.y, rt2)) * viewSize + offset;

                  if (distance(io.MousePos.x, io.MousePos.y, pos1.x, pos1.y, pos2.x, pos2.y) < 8.f && !scrollingV)
                  {
                     localOverCurve = int(c);
                     overCurve = int(c);
                     overCurveOrPoint = true;
                  }

                  draw_list->AddLine(pos1, pos2, curveColor, 1.3f);
               } // substep
            }
            else if (curveType == CurveDiscrete)
            {
               ImVec2 dp1 = p1 * viewSize + offset;
               ImVec2 dp2 = ImVec2(p2.x, p1.y) * viewSize + offset;
               ImVec2 dp3 = p2 * viewSize + offset;
               draw_list->AddLine(dp1, dp2, curveColor, 1.3f);
               draw_list->AddLine(dp2, dp3, curveColor, 1.3f);

               if ((distance(io.MousePos.x, io.MousePos.y, dp1.x, dp1.y, dp3.x, dp1.y) < 8.f ||
                  distance(io.MousePos.x, io.MousePos.y, dp3.x, dp1.y, dp3.x, dp3.y) < 8.f)
                  /*&& localOverCurve == -1*/)
               {
                  localOverCurve = int(c);
                  overCurve = int(c);
                  overCurveOrPoint = true;
               }
            }
         } // point loop

         for (size_t p = 0; p < ptCount; p++)
         {
            const int drawState = DrawPoint(draw_list, pointToRange(pts[p]), viewSize, offset, (selection.find({ int(c), int(p) }) != selection.end() && movingCurve == -1 && !scrollingV));
            if (drawState && movingCurve == -1 && !selectingQuad)
            {
               overCurveOrPoint = true;
               overSelectedPoint = true;
               overCurve = -1;
               if (drawState == 2)
               {
                  if (!io.KeyShift && selection.find({ int(c), int(p) }) == selection.end())
                     selection.clear();
                  selection.insert({ int(c), int(p) });
               }
            }
         }
      } // curves loop

      if (localOverCurve == -1)
         overCurve = -1;

      // move selection
      static bool pointsMoved = false;
      static ImVec2 mousePosOrigin;
      static std::vector<ImVec2> originalPoints;
      if (overSelectedPoint && io.MouseDown[0])
      {
         if ((fabsf(io.MouseDelta.x) > 0.f || fabsf(io.MouseDelta.y) > 0.f) && !selection.empty())
         {
            if (!pointsMoved)
            {
               delegate.BeginEdit(0);
               mousePosOrigin = io.MousePos;
               originalPoints.resize(selection.size());
               int index = 0;
               for (auto& sel : selection)
               {
                  const ImVec2* pts = delegate.GetPoints(sel.curveIndex);
                  originalPoints[index++] = pts[sel.pointIndex];
               }
            }
            pointsMoved = true;
            ret = 1;
            auto prevSelection = selection;
            int originalIndex = 0;
            for (auto& sel : prevSelection)
            {
               const ImVec2 p = rangeToPoint(pointToRange(originalPoints[originalIndex]) + (io.MousePos - mousePosOrigin) * sizeOfPixel);
               const int newIndex = delegate.EditPoint(sel.curveIndex, sel.pointIndex, p);
               if (newIndex != sel.pointIndex)
               {
                  selection.erase(sel);
                  selection.insert({ sel.curveIndex, newIndex });
               }
               originalIndex++;
            }
         }
      }

      if (overSelectedPoint && !io.MouseDown[0])
      {
         overSelectedPoint = false;
         if (pointsMoved)
         {
            pointsMoved = false;
            delegate.EndEdit();
         }
      }

      // add point
      if (overCurve != -1 && io.MouseDoubleClicked[0])
      {
         const ImVec2 np = rangeToPoint((io.MousePos - offset) / viewSize);
         delegate.BeginEdit(overCurve);
         delegate.AddPoint(overCurve, np);
         delegate.EndEdit();
         ret = 1;
      }

      // move curve

      if (movingCurve != -1)
      {
         const size_t ptCount = delegate.GetPointCount(movingCurve);
         const ImVec2* pts = delegate.GetPoints(movingCurve);
         if (!pointsMoved)
         {
            mousePosOrigin = io.MousePos;
            pointsMoved = true;
            originalPoints.resize(ptCount);
            for (size_t index = 0; index < ptCount; index++)
            {
               originalPoints[index] = pts[index];
            }
         }
         if (ptCount >= 1)
         {
            for (size_t p = 0; p < ptCount; p++)
            {
               delegate.EditPoint(movingCurve, int(p), rangeToPoint(pointToRange(originalPoints[p]) + (io.MousePos - mousePosOrigin) * sizeOfPixel));
            }
            ret = 1;
         }
         if (!io.MouseDown[0])
         {
            movingCurve = -1;
            pointsMoved = false;
            delegate.EndEdit();
         }
      }
      if (movingCurve == -1 && overCurve != -1 && ImGui::IsMouseClicked(0) && selection.empty() && !selectingQuad)
      {
         movingCurve = overCurve;
         delegate.BeginEdit(overCurve);
      }

      // quad selection
      if (selectingQuad)
      {
         const ImVec2 bmin = ImMin(quadSelection, io.MousePos);
         const ImVec2 bmax = ImMax(quadSelection, io.MousePos);
         draw_list->AddRectFilled(bmin, bmax, 0x40FF0000, 1.f);
         draw_list->AddRect(bmin, bmax, 0xFFFF0000, 1.f);
         const ImRect selectionQuad(bmin, bmax);
         if (!io.MouseDown[0])
         {
            if (!io.KeyShift)
               selection.clear();
            // select everythnig is quad
            for (size_t c = 0; c < curveCount; c++)
            {
               if (!delegate.IsVisible(c))
                  continue;

               const size_t ptCount = delegate.GetPointCount(c);
               if (ptCount < 1)
                  continue;

               const ImVec2* pts = delegate.GetPoints(c);
               for (size_t p = 0; p < ptCount; p++)
               {
                  const ImVec2 center = pointToRange(pts[p]) * viewSize + offset;
                  if (selectionQuad.Contains(center))
                     selection.insert({ int(c), int(p) });
               }
            }
            // done
            selectingQuad = false;
         }
      }
      if (!overCurveOrPoint && ImGui::IsMouseClicked(0) && !selectingQuad && movingCurve == -1 && !overSelectedPoint && container.Contains(io.MousePos))
      {
         selectingQuad = true;
         quadSelection = io.MousePos;
      }
      if (clippingRect)
         draw_list->PopClipRect();

      ImGui::EndChild();
      ImGui::PopStyleVar();
      ImGui::PopStyleColor(1);

      if (selectedPoints)
      {
         selectedPoints->resize(int(selection.size()));
         int index = 0;
         for (auto& point : selection)
            (*selectedPoints)[index++] = point;
      }
      return ret;
   }
}

// https://github.com/CedricGuillemet/ImGuizmo
// v1.91.3 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once

#include <cstddef>

struct ImDrawList;
struct ImRect;
namespace ImSequencer
{
   enum SEQUENCER_OPTIONS
   {
      SEQUENCER_EDIT_NONE = 0,
      SEQUENCER_EDIT_STARTEND = 1 << 1,
      SEQUENCER_CHANGE_FRAME = 1 << 3,
      SEQUENCER_ADD = 1 << 4,
      SEQUENCER_DEL = 1 << 5,
      SEQUENCER_COPYPASTE = 1 << 6,
      SEQUENCER_EDIT_ALL = SEQUENCER_EDIT_STARTEND | SEQUENCER_CHANGE_FRAME
   };

   struct SequenceInterface
   {
      bool focused = false;
      virtual int GetFrameMin() const = 0;
      virtual int GetFrameMax() const = 0;
      virtual int GetItemCount() const = 0;

      virtual void BeginEdit(int /*index*/) {}
      virtual void EndEdit() {}
      virtual int GetItemTypeCount() const { return 0; }
      virtual const char* GetItemTypeName(int /*typeIndex*/) const { return ""; }
      virtual const char* GetItemLabel(int /*index*/) const { return ""; }
      virtual const char* GetCollapseFmt() const { return "%d Frames / %d entries"; }

      virtual void Get(int index, int** start, int** end, int* type, unsigned int* color) = 0;
      virtual void Add(int /*type*/) {}
      virtual void Del(int /*index*/) {}
      virtual void Duplicate(int /*index*/) {}

      virtual void Copy() {}
      virtual void Paste() {}

      virtual size_t GetCustomHeight(int /*index*/) { return 0; }
      virtual void DoubleClick(int /*index*/) {}
      virtual void CustomDraw(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*legendRect*/, const ImRect& /*clippingRect*/, const ImRect& /*legendClippingRect*/) {}
      virtual void CustomDrawCompact(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*clippingRect*/) {}

       virtual ~SequenceInterface() = default;
   };


   // return true if selection is made
   bool Sequencer(SequenceInterface* sequence, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions);

}

// https://github.com/CedricGuillemet/ImGuizmo
// v1.91.3 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//#include "ImSequencer.h"
//#include "imgui.h"
//#include "imgui_internal.h"
#include <cstdlib>

namespace ImSequencer
{
#ifndef IMGUI_DEFINE_MATH_OPERATORS
   static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
      return ImVec2(a.x + b.x, a.y + b.y);
   }
#endif
   static bool SequencerAddDelButton(ImDrawList* draw_list, ImVec2 pos, bool add = true)
   {
      ImGuiIO& io = ImGui::GetIO();
      ImRect btnRect(pos, ImVec2(pos.x + 16, pos.y + 16));
      bool overBtn = btnRect.Contains(io.MousePos);
      bool containedClick = overBtn && btnRect.Contains(io.MouseClickedPos[0]);
      bool clickedBtn = containedClick && io.MouseReleased[0];
      int btnColor = overBtn ? 0xAAEAFFAA : 0x77A3B2AA;
      if (containedClick && io.MouseDownDuration[0] > 0)
         btnRect.Expand(2.0f);

      float midy = pos.y + 16 / 2 - 0.5f;
      float midx = pos.x + 16 / 2 - 0.5f;
      draw_list->AddRect(btnRect.Min, btnRect.Max, btnColor, 4);
      draw_list->AddLine(ImVec2(btnRect.Min.x + 3, midy), ImVec2(btnRect.Max.x - 3, midy), btnColor, 2);
      if (add)
         draw_list->AddLine(ImVec2(midx, btnRect.Min.y + 3), ImVec2(midx, btnRect.Max.y - 3), btnColor, 2);
      return clickedBtn;
   }

   bool Sequencer(SequenceInterface* sequence, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions)
   {
      bool ret = false;
      ImGuiIO& io = ImGui::GetIO();
      int cx = (int)(io.MousePos.x);
      int cy = (int)(io.MousePos.y);
      static float framePixelWidth = 10.f;
      static float framePixelWidthTarget = 10.f;
      int legendWidth = 200;

      static int movingEntry = -1;
      static int movingPos = -1;
      static int movingPart = -1;
      int delEntry = -1;
      int dupEntry = -1;
      int ItemHeight = 20;

      bool popupOpened = false;
      int sequenceCount = sequence->GetItemCount();
      if (!sequenceCount)
         return false;
      ImGui::BeginGroup();

      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
      ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
      int firstFrameUsed = firstFrame ? *firstFrame : 0;


      int controlHeight = sequenceCount * ItemHeight;
      for (int i = 0; i < sequenceCount; i++)
         controlHeight += int(sequence->GetCustomHeight(i));
      int frameCount = ImMax(sequence->GetFrameMax() - sequence->GetFrameMin(), 1);

      static bool MovingScrollBar = false;
      static bool MovingCurrentFrame = false;
      struct CustomDraw
      {
         int index;
         ImRect customRect;
         ImRect legendRect;
         ImRect clippingRect;
         ImRect legendClippingRect;
      };
      ImVector<CustomDraw> customDraws;
      ImVector<CustomDraw> compactCustomDraws;
      // zoom in/out
      const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
      const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
      const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

      ImRect regionRect(canvas_pos, canvas_pos + canvas_size);

      static bool panningView = false;
      static ImVec2 panningViewSource;
      static int panningViewFrame;
      if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2])
      {
         if (!panningView)
         {
            panningViewSource = io.MousePos;
            panningView = true;
            panningViewFrame = *firstFrame;
         }
         *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
         *firstFrame = ImClamp(*firstFrame, sequence->GetFrameMin(), sequence->GetFrameMax() - visibleFrameCount);
      }
      if (panningView && !io.MouseDown[2])
      {
         panningView = false;
      }
      framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

      framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

      frameCount = sequence->GetFrameMax() - sequence->GetFrameMin();
      if (visibleFrameCount >= frameCount && firstFrame)
         *firstFrame = sequence->GetFrameMin();


      // --
      if (expanded && !*expanded)
      {
         ImGui::InvisibleButton("canvas", ImVec2(canvas_size.x - canvas_pos.x, (float)ItemHeight));
         draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
         char tmps[512];
         ImFormatString(tmps, IM_ARRAYSIZE(tmps), sequence->GetCollapseFmt(), frameCount, sequenceCount);
         draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
      }
      else
      {
         bool hasScrollBar(true);
         /*
         int framesPixelWidth = int(frameCount * framePixelWidth);
         if ((framesPixelWidth + legendWidth) >= canvas_size.x)
         {
             hasScrollBar = true;
         }
         */
         // test scroll area
         ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
         ImVec2 scrollBarSize(canvas_size.x, 14.f);
         ImGui::InvisibleButton("topBar", headerSize);
         draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
         ImVec2 childFramePos = ImGui::GetCursorScreenPos();
         ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));
         ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
         ImGui::BeginChild(889, childFrameSize, ImGuiChildFlags_FrameStyle);
         sequence->focused = ImGui::IsWindowFocused();
         ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, float(controlHeight)));
         const ImVec2 contentMin = ImGui::GetItemRectMin();
         const ImVec2 contentMax = ImGui::GetItemRectMax();
         const ImRect contentRect(contentMin, contentMax);
         const float contentHeight = contentMax.y - contentMin.y;

         // full background
         draw_list->AddRectFilled(canvas_pos, canvas_pos + canvas_size, 0xFF242424, 0);

         // current frame top
         ImRect topRect(ImVec2(canvas_pos.x + legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + ItemHeight));

         if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && sequenceOptions & SEQUENCER_CHANGE_FRAME && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0])
         {
            MovingCurrentFrame = true;
         }
         if (MovingCurrentFrame)
         {
            if (frameCount)
            {
               *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
               if (*currentFrame < sequence->GetFrameMin())
                  *currentFrame = sequence->GetFrameMin();
               if (*currentFrame >= sequence->GetFrameMax())
                  *currentFrame = sequence->GetFrameMax();
            }
            if (!io.MouseDown[0])
               MovingCurrentFrame = false;
         }

         //header
         draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
         if (sequenceOptions & SEQUENCER_ADD)
         {
            if (SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + legendWidth - ItemHeight, canvas_pos.y + 2), true))
               ImGui::OpenPopup("addEntry");

            if (ImGui::BeginPopup("addEntry"))
            {
               for (int i = 0; i < sequence->GetItemTypeCount(); i++)
                  if (ImGui::Selectable(sequence->GetItemTypeName(i)))
                  {
                     sequence->Add(i);
                     *selectedEntry = sequence->GetItemCount() - 1;
                  }

               ImGui::EndPopup();
               popupOpened = true;
            }
         }

         //header frame number and lines
         int modFrameCount = 10;
         int frameStep = 1;
         while ((modFrameCount * framePixelWidth) < 150)
         {
            modFrameCount *= 2;
            frameStep *= 2;
         };
         int halfModFrameCount = modFrameCount / 2;

         auto drawLine = [&](int i, int regionHeight) {
            bool baseIndex = ((i % modFrameCount) == 0) || (i == sequence->GetFrameMax() || i == sequence->GetFrameMin());
            bool halfIndex = (i % halfModFrameCount) == 0;
            int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth - int(firstFrameUsed * framePixelWidth);
            int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
            int tiretEnd = baseIndex ? regionHeight : ItemHeight;

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
            {
               draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

               draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
            }

            if (baseIndex && px > (canvas_pos.x + legendWidth))
            {
               char tmps[512];
               ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
               draw_list->AddText(ImVec2((float)px + 3.f, canvas_pos.y), 0xFFBBBBBB, tmps);
            }

         };

         auto drawLineContent = [&](int i, int /*regionHeight*/) {
            int px = (int)canvas_pos.x + int(i * framePixelWidth) + legendWidth - int(firstFrameUsed * framePixelWidth);
            int tiretStart = int(contentMin.y);
            int tiretEnd = int(contentMax.y);

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth))
            {
               //draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

               draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
            }
         };
         for (int i = sequence->GetFrameMin(); i <= sequence->GetFrameMax(); i += frameStep)
         {
            drawLine(i, ItemHeight);
         }
         drawLine(sequence->GetFrameMin(), ItemHeight);
         drawLine(sequence->GetFrameMax(), ItemHeight);
         /*
                  draw_list->AddLine(canvas_pos, ImVec2(canvas_pos.x, canvas_pos.y + controlHeight), 0xFF000000, 1);
                  draw_list->AddLine(ImVec2(canvas_pos.x, canvas_pos.y + ItemHeight), ImVec2(canvas_size.x, canvas_pos.y + ItemHeight), 0xFF000000, 1);
                  */
                  // clip content

         draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

         // draw item names in the legend rect on the left
         size_t customHeight = 0;
         for (int i = 0; i < sequenceCount; i++)
         {
            int type;
            sequence->Get(i, NULL, NULL, &type, NULL);
            ImVec2 tpos(contentMin.x + 3, contentMin.y + i * ItemHeight + 2 + customHeight);
            draw_list->AddText(tpos, 0xFFFFFFFF, sequence->GetItemLabel(i));

            if (sequenceOptions & SEQUENCER_DEL)
            {
               if (SequencerAddDelButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight + 2 - 10, tpos.y + 2), false))
                  delEntry = i;

               if (SequencerAddDelButton(draw_list, ImVec2(contentMin.x + legendWidth - ItemHeight - ItemHeight + 2 - 10, tpos.y + 2), true))
                  dupEntry = i;
            }
            customHeight += sequence->GetCustomHeight(i);
         }

         // slots background
         customHeight = 0;
         for (int i = 0; i < sequenceCount; i++)
         {
            unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

            size_t localCustomHeight = sequence->GetCustomHeight(i);
            ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + ItemHeight * i + 1 + customHeight);
            ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, pos.y + ItemHeight - 1 + localCustomHeight);
            if (!popupOpened && cy >= pos.y && cy < pos.y + (ItemHeight + localCustomHeight) && movingEntry == -1 && cx>contentMin.x && cx < contentMin.x + canvas_size.x)
            {
               col += 0x80201008;
               pos.x -= legendWidth;
            }
            draw_list->AddRectFilled(pos, sz, col, 0);
            customHeight += localCustomHeight;
         }

         draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);

         // vertical frame lines in content area
         for (int i = sequence->GetFrameMin(); i <= sequence->GetFrameMax(); i += frameStep)
         {
            drawLineContent(i, int(contentHeight));
         }
         drawLineContent(sequence->GetFrameMin(), int(contentHeight));
         drawLineContent(sequence->GetFrameMax(), int(contentHeight));

         // selection
         bool selected = selectedEntry && (*selectedEntry >= 0);
         if (selected)
         {
            customHeight = 0;
            for (int i = 0; i < *selectedEntry; i++)
               customHeight += sequence->GetCustomHeight(i);
            draw_list->AddRectFilled(ImVec2(contentMin.x, contentMin.y + ItemHeight * *selectedEntry + customHeight), ImVec2(contentMin.x + canvas_size.x, contentMin.y + ItemHeight * (*selectedEntry + 1) + customHeight), 0x801080FF, 1.f);
         }

         // slots
         customHeight = 0;
         for (int i = 0; i < sequenceCount; i++)
         {
            int* start, * end;
            unsigned int color;
            sequence->Get(i, &start, &end, NULL, &color);
            size_t localCustomHeight = sequence->GetCustomHeight(i);

            ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * i + 1 + customHeight);
            ImVec2 slotP1(pos.x + *start * framePixelWidth, pos.y + 2);
            ImVec2 slotP2(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + ItemHeight - 2);
            ImVec2 slotP3(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + ItemHeight - 2 + localCustomHeight);
            unsigned int slotColor = color | 0xFF000000;
            unsigned int slotColorHalf = (color & 0xFFFFFF) | 0x40000000;

            if (slotP1.x <= (canvas_size.x + contentMin.x) && slotP2.x >= (contentMin.x + legendWidth))
            {
               draw_list->AddRectFilled(slotP1, slotP3, slotColorHalf, 2);
               draw_list->AddRectFilled(slotP1, slotP2, slotColor, 2);
            }
            if (ImRect(slotP1, slotP2).Contains(io.MousePos) && io.MouseDoubleClicked[0])
            {
               sequence->DoubleClick(i);
            }
            // Ensure grabbable handles
            const float max_handle_width = slotP2.x - slotP1.x / 3.0f;
            const float min_handle_width = ImMin(10.0f, max_handle_width);
            const float handle_width = ImClamp(framePixelWidth / 2.0f, min_handle_width, max_handle_width);
            ImRect rects[3] = { ImRect(slotP1, ImVec2(slotP1.x + handle_width, slotP2.y))
                , ImRect(ImVec2(slotP2.x - handle_width, slotP1.y), slotP2)
                , ImRect(slotP1, slotP2) };

            const unsigned int quadColor[] = { 0xFFFFFFFF, 0xFFFFFFFF, slotColor + (selected ? 0 : 0x202020) };
            if (movingEntry == -1 && (sequenceOptions & SEQUENCER_EDIT_STARTEND))// TODOFOCUS && backgroundRect.Contains(io.MousePos))
            {
               for (int j = 2; j >= 0; j--)
               {
                  ImRect& rc = rects[j];
                  if (!rc.Contains(io.MousePos))
                     continue;
                  draw_list->AddRectFilled(rc.Min, rc.Max, quadColor[j], 2);
               }

               for (int j = 0; j < 3; j++)
               {
                  ImRect& rc = rects[j];
                  if (!rc.Contains(io.MousePos))
                     continue;
                  if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                     continue;
                  if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame)
                  {
                     movingEntry = i;
                     movingPos = cx;
                     movingPart = j + 1;
                     sequence->BeginEdit(movingEntry);
                     break;
                  }
               }
            }

            // custom draw
            if (localCustomHeight > 0)
            {
               ImVec2 rp(canvas_pos.x, contentMin.y + ItemHeight * i + 1 + customHeight);
               ImRect customRect(rp + ImVec2(legendWidth - (firstFrameUsed - sequence->GetFrameMin() - 0.5f) * framePixelWidth, float(ItemHeight)),
                  rp + ImVec2(legendWidth + (sequence->GetFrameMax() - firstFrameUsed - 0.5f + 2.f) * framePixelWidth, float(localCustomHeight + ItemHeight)));
               ImRect clippingRect(rp + ImVec2(float(legendWidth), float(ItemHeight)), rp + ImVec2(canvas_size.x, float(localCustomHeight + ItemHeight)));

               ImRect legendRect(rp + ImVec2(0.f, float(ItemHeight)), rp + ImVec2(float(legendWidth), float(localCustomHeight)));
               ImRect legendClippingRect(canvas_pos + ImVec2(0.f, float(ItemHeight)), canvas_pos + ImVec2(float(legendWidth), float(localCustomHeight + ItemHeight)));
               customDraws.push_back({ i, customRect, legendRect, clippingRect, legendClippingRect });
            }
            else
            {
               ImVec2 rp(canvas_pos.x, contentMin.y + ItemHeight * i + customHeight);
               ImRect customRect(rp + ImVec2(legendWidth - (firstFrameUsed - sequence->GetFrameMin() - 0.5f) * framePixelWidth, float(0.f)),
                  rp + ImVec2(legendWidth + (sequence->GetFrameMax() - firstFrameUsed - 0.5f + 2.f) * framePixelWidth, float(ItemHeight)));
               ImRect clippingRect(rp + ImVec2(float(legendWidth), float(0.f)), rp + ImVec2(canvas_size.x, float(ItemHeight)));

               compactCustomDraws.push_back({ i, customRect, ImRect(), clippingRect, ImRect() });
            }
            customHeight += localCustomHeight;
         }


         // moving
         if (/*backgroundRect.Contains(io.MousePos) && */movingEntry >= 0)
         {
#if IMGUI_VERSION_NUM >= 18723
            ImGui::SetNextFrameWantCaptureMouse(true);
#else
            ImGui::CaptureMouseFromApp();
#endif
            int diffFrame = int((cx - movingPos) / framePixelWidth);
            if (std::abs(diffFrame) > 0)
            {
               int* start, * end;
               sequence->Get(movingEntry, &start, &end, NULL, NULL);
               if (selectedEntry)
                  *selectedEntry = movingEntry;
               int& l = *start;
               int& r = *end;
               if (movingPart & 1)
                  l += diffFrame;
               if (movingPart & 2)
                  r += diffFrame;
               if (l < 0)
               {
                  if (movingPart & 2)
                     r -= l;
                  l = 0;
               }
               if (movingPart & 1 && l > r)
                  l = r;
               if (movingPart & 2 && r < l)
                  r = l;
               movingPos += int(diffFrame * framePixelWidth);
            }
            if (!io.MouseDown[0])
            {
               // single select
               if (!diffFrame && movingPart && selectedEntry)
               {
                  *selectedEntry = movingEntry;
                  ret = true;
               }

               movingEntry = -1;
               sequence->EndEdit();
            }
         }

         // cursor
         if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= sequence->GetFrameMax())
         {
            static const float cursorWidth = 8.f;
            float cursorOffset = contentMin.x + legendWidth + (*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2 - cursorWidth * 0.5f;
            draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, cursorWidth);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", *currentFrame);
            draw_list->AddText(ImVec2(cursorOffset + 10, canvas_pos.y + 2), 0xFF2A2AFF, tmps);
         }

         draw_list->PopClipRect();
         draw_list->PopClipRect();

         for (auto& customDraw : customDraws)
            sequence->CustomDraw(customDraw.index, draw_list, customDraw.customRect, customDraw.legendRect, customDraw.clippingRect, customDraw.legendClippingRect);
         for (auto& customDraw : compactCustomDraws)
            sequence->CustomDrawCompact(customDraw.index, draw_list, customDraw.customRect, customDraw.clippingRect);

         // copy paste
         if (sequenceOptions & SEQUENCER_COPYPASTE)
         {
            ImRect rectCopy(ImVec2(contentMin.x + 100, canvas_pos.y + 2)
               , ImVec2(contentMin.x + 100 + 30, canvas_pos.y + ItemHeight - 2));
            bool inRectCopy = rectCopy.Contains(io.MousePos);
            unsigned int copyColor = inRectCopy ? 0xFF1080FF : 0xFF000000;
            draw_list->AddText(rectCopy.Min, copyColor, "Copy");

            ImRect rectPaste(ImVec2(contentMin.x + 140, canvas_pos.y + 2)
               , ImVec2(contentMin.x + 140 + 30, canvas_pos.y + ItemHeight - 2));
            bool inRectPaste = rectPaste.Contains(io.MousePos);
            unsigned int pasteColor = inRectPaste ? 0xFF1080FF : 0xFF000000;
            draw_list->AddText(rectPaste.Min, pasteColor, "Paste");

            if (inRectCopy && io.MouseReleased[0])
            {
               sequence->Copy();
            }
            if (inRectPaste && io.MouseReleased[0])
            {
               sequence->Paste();
            }
         }
         //

         ImGui::EndChild();
         ImGui::PopStyleColor();
         if (hasScrollBar)
         {
            ImGui::InvisibleButton("scrollBar", scrollBarSize);
            ImVec2 scrollBarMin = ImGui::GetItemRectMin();
            ImVec2 scrollBarMax = ImGui::GetItemRectMax();

            // ratio = number of frames visible in control / number to total frames

            float startFrameOffset = ((float)(firstFrameUsed - sequence->GetFrameMin()) / (float)frameCount) * (canvas_size.x - legendWidth);
            ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
            ImVec2 scrollBarB(scrollBarMin.x + canvas_size.x, scrollBarMax.y - 1);
            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

            ImRect scrollBarRect(scrollBarA, scrollBarB);
            bool inScrollBar = scrollBarRect.Contains(io.MousePos);

            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);


            ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
            ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
            draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

            ImRect barHandleLeft(scrollBarC, ImVec2(scrollBarC.x + 14, scrollBarD.y));
            ImRect barHandleRight(ImVec2(scrollBarD.x - 14, scrollBarC.y), scrollBarD);

            bool onLeft = barHandleLeft.Contains(io.MousePos);
            bool onRight = barHandleRight.Contains(io.MousePos);

            static bool sizingRBar = false;
            static bool sizingLBar = false;

            draw_list->AddRectFilled(barHandleLeft.Min, barHandleLeft.Max, (onLeft || sizingLBar) ? 0xFFAAAAAA : 0xFF666666, 6);
            draw_list->AddRectFilled(barHandleRight.Min, barHandleRight.Max, (onRight || sizingRBar) ? 0xFFAAAAAA : 0xFF666666, 6);

            ImRect scrollBarThumb(scrollBarC, scrollBarD);
            static const float MinBarWidth = 44.f;
            if (sizingRBar)
            {
               if (!io.MouseDown[0])
               {
                  sizingRBar = false;
               }
               else
               {
                  float barNewWidth = ImMax(barWidthInPixels + io.MouseDelta.x, MinBarWidth);
                  float barRatio = barNewWidth / barWidthInPixels;
                  framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                  int newVisibleFrameCount = int((canvas_size.x - legendWidth) / framePixelWidthTarget);
                  int lastFrame = *firstFrame + newVisibleFrameCount;
                  if (lastFrame > sequence->GetFrameMax())
                  {
                     framePixelWidthTarget = framePixelWidth = (canvas_size.x - legendWidth) / float(sequence->GetFrameMax() - *firstFrame);
                  }
               }
            }
            else if (sizingLBar)
            {
               if (!io.MouseDown[0])
               {
                  sizingLBar = false;
               }
               else
               {
                  if (fabsf(io.MouseDelta.x) > FLT_EPSILON)
                  {
                     float barNewWidth = ImMax(barWidthInPixels - io.MouseDelta.x, MinBarWidth);
                     float barRatio = barNewWidth / barWidthInPixels;
                     float previousFramePixelWidthTarget = framePixelWidthTarget;
                     framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                     int newVisibleFrameCount = int(visibleFrameCount / barRatio);
                     int newFirstFrame = *firstFrame + newVisibleFrameCount - visibleFrameCount;
                     newFirstFrame = ImClamp(newFirstFrame, sequence->GetFrameMin(), ImMax(sequence->GetFrameMax() - visibleFrameCount, sequence->GetFrameMin()));
                     if (newFirstFrame == *firstFrame)
                     {
                        framePixelWidth = framePixelWidthTarget = previousFramePixelWidthTarget;
                     }
                     else
                     {
                        *firstFrame = newFirstFrame;
                     }
                  }
               }
            }
            else
            {
               if (MovingScrollBar)
               {
                  if (!io.MouseDown[0])
                  {
                     MovingScrollBar = false;
                  }
                  else
                  {
                     float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                     *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                     *firstFrame = ImClamp(*firstFrame, sequence->GetFrameMin(), ImMax(sequence->GetFrameMax() - visibleFrameCount, sequence->GetFrameMin()));
                  }
               }
               else
               {
                  if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingEntry == -1)
                  {
                     MovingScrollBar = true;
                     panningViewSource = io.MousePos;
                     panningViewFrame = -*firstFrame;
                  }
                  if (!sizingRBar && onRight && ImGui::IsMouseClicked(0))
                     sizingRBar = true;
                  if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0))
                     sizingLBar = true;

               }
            }
         }
      }

      ImGui::EndGroup();

      if (regionRect.Contains(io.MousePos))
      {
         bool overCustomDraw = false;
         for (auto& custom : customDraws)
         {
            if (custom.customRect.Contains(io.MousePos))
            {
               overCustomDraw = true;
            }
         }
         if (overCustomDraw)
         {
         }
         else
         {
#if 0
            frameOverCursor = *firstFrame + (int)(visibleFrameCount * ((io.MousePos.x - (float)legendWidth - canvas_pos.x) / (canvas_size.x - legendWidth)));
            //frameOverCursor = max(min(*firstFrame - visibleFrameCount / 2, frameCount - visibleFrameCount), 0);

            /**firstFrame -= frameOverCursor;
            *firstFrame *= framePixelWidthTarget / framePixelWidth;
            *firstFrame += frameOverCursor;*/
            if (io.MouseWheel < -FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 1.1f);
               framePixelWidthTarget *= 0.9f;
               *firstFrame += frameOverCursor;
            }

            if (io.MouseWheel > FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 0.9f);
               framePixelWidthTarget *= 1.1f;
               *firstFrame += frameOverCursor;
            }
#endif
         }
      }

      if (expanded)
      {
         if (SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), !*expanded))
            *expanded = !*expanded;
      }

      if (delEntry != -1)
      {
         sequence->Del(delEntry);
         if (selectedEntry && (*selectedEntry == delEntry || *selectedEntry >= sequence->GetItemCount()))
            *selectedEntry = -1;
      }

      if (dupEntry != -1)
      {
         sequence->Duplicate(dupEntry);
      }
      return ret;
   }
}

// demo

//
//
// ImSequencer interface
//
//
static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

struct RampEdit : public ImCurveEdit::Delegate
{
   RampEdit()
   {
      mPts[0][0] = ImVec2(-10.f, 0);
      mPts[0][1] = ImVec2(20.f, 0.6f);
      mPts[0][2] = ImVec2(25.f, 0.2f);
      mPts[0][3] = ImVec2(70.f, 0.4f);
      mPts[0][4] = ImVec2(120.f, 1.f);
      mPointCount[0] = 5;

      mPts[1][0] = ImVec2(-50.f, 0.2f);
      mPts[1][1] = ImVec2(33.f, 0.7f);
      mPts[1][2] = ImVec2(80.f, 0.2f);
      mPts[1][3] = ImVec2(82.f, 0.8f);
      mPointCount[1] = 4;


      mPts[2][0] = ImVec2(40.f, 0);
      mPts[2][1] = ImVec2(60.f, 0.1f);
      mPts[2][2] = ImVec2(90.f, 0.82f);
      mPts[2][3] = ImVec2(150.f, 0.24f);
      mPts[2][4] = ImVec2(200.f, 0.34f);
      mPts[2][5] = ImVec2(250.f, 0.12f);
      mPointCount[2] = 6;
      mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
      mMax = ImVec2(1.f, 1.f);
      mMin = ImVec2(0.f, 0.f);
   }
   size_t GetCurveCount()
   {
      return 3;
   }

   bool IsVisible(size_t curveIndex)
   {
      return mbVisible[curveIndex];
   }
   size_t GetPointCount(size_t curveIndex)
   {
      return mPointCount[curveIndex];
   }

   uint32_t GetCurveColor(size_t curveIndex)
   {
      uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
      return cols[curveIndex];
   }
   ImVec2* GetPoints(size_t curveIndex)
   {
      return mPts[curveIndex];
   }
   virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
   virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
   {
      mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
      SortValues(curveIndex);
      for (size_t i = 0; i < GetPointCount(curveIndex); i++)
      {
         if (mPts[curveIndex][i].x == value.x)
            return (int)i;
      }
      return pointIndex;
   }
   virtual void AddPoint(size_t curveIndex, ImVec2 value)
   {
      if (mPointCount[curveIndex] >= 8)
         return;
      mPts[curveIndex][mPointCount[curveIndex]++] = value;
      SortValues(curveIndex);
   }
   virtual ImVec2& GetMax() { return mMax; }
   virtual ImVec2& GetMin() { return mMin; }
   virtual unsigned int GetBackgroundColor() { return 0; }
   ImVec2 mPts[3][8];
   size_t mPointCount[3];
   bool mbVisible[3];
   ImVec2 mMin;
   ImVec2 mMax;
private:
   void SortValues(size_t curveIndex)
   {
      auto b = std::begin(mPts[curveIndex]);
      auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
      std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

   }
};

struct MySequence : public ImSequencer::SequenceInterface
{
   // interface with sequencer

   virtual int GetFrameMin() const {
      return mFrameMin;
   }
   virtual int GetFrameMax() const {
      return mFrameMax;
   }
   virtual int GetItemCount() const { return (int)myItems.size(); }

   virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
   virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
   virtual const char* GetItemLabel(int index) const
   {
      static char tmps[512];
      snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
      return tmps;
   }

   virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
   {
      MySequenceItem& item = myItems[index];
      if (color)
         *color = 0xFFAA8080; // same color for everyone, return color based on type
      if (start)
         *start = &item.mFrameStart;
      if (end)
         *end = &item.mFrameEnd;
      if (type)
         *type = item.mType;
   }
   virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
   virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
   virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

   virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

   // my datas
   MySequence() : mFrameMin(0), mFrameMax(0) {}
   int mFrameMin, mFrameMax;
   struct MySequenceItem
   {
      int mType;
      int mFrameStart, mFrameEnd;
      bool mExpanded;
   };
   std::vector<MySequenceItem> myItems;
   RampEdit rampEdit;

   virtual void DoubleClick(int index) {
      if (myItems[index].mExpanded)
      {
         myItems[index].mExpanded = false;
         return;
      }
      for (auto& item : myItems)
         item.mExpanded = false;
      myItems[index].mExpanded = !myItems[index].mExpanded;
   }

   virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
   {
      static const char* labels[] = { "Translation", "Rotation" , "Scale" };

      rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
      rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
      draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
      for (int i = 0; i < 3; i++)
      {
         ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
         ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
         draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
         if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
            rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
      }
      draw_list->PopClipRect();

      ImGui::SetCursorScreenPos(rc.Min);
      ImCurveEdit::Edit(rampEdit, rc.Max - rc.Min, 137 + index, &clippingRect);
   }

   virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
   {
      rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
      rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
      draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
      for (int i = 0; i < 3; i++)
      {
         for (unsigned int j = 0; j < rampEdit.mPointCount[i]; j++)
         {
            float p = rampEdit.mPts[i][j].x;
            if (p < myItems[index].mFrameStart || p > myItems[index].mFrameEnd)
               continue;
            float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
            float x = ImLerp(rc.Min.x, rc.Max.x, r);
            draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
         }
      }
      draw_list->PopClipRect();
   }
};


extern "C"
void igSequencerDemo() {
   // sequence with default values
   static MySequence mySequence;
   static int once = 0; if( !once ) { once = 1;
   mySequence.mFrameMin = -100;
   mySequence.mFrameMax = 1000;
   mySequence.myItems.push_back(MySequence::MySequenceItem{ 0, 10, 30, false });
   mySequence.myItems.push_back(MySequence::MySequenceItem{ 1, 20, 30, true });
   mySequence.myItems.push_back(MySequence::MySequenceItem{ 3, 12, 60, false });
   mySequence.myItems.push_back(MySequence::MySequenceItem{ 2, 61, 90, false });
   mySequence.myItems.push_back(MySequence::MySequenceItem{ 4, 90, 99, false });
   }

   if (ImGui::CollapsingHeader("Sequencer"))
   {
      // let's create the sequencer
      static int selectedEntry = -1;
      static int firstFrame = 0;
      static bool expanded = true;
      static int currentFrame = 100;

      ImGui::PushItemWidth(130);
      ImGui::InputInt("Frame Min", &mySequence.mFrameMin);
      ImGui::SameLine();
      ImGui::InputInt("Frame ", &currentFrame);
      ImGui::SameLine();
      ImGui::InputInt("Frame Max", &mySequence.mFrameMax);
      ImGui::PopItemWidth();
      Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);
      // add a UI to edit that particular item
      if (selectedEntry != -1)
      {
        const MySequence::MySequenceItem &item = mySequence.myItems[selectedEntry];
        ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
        // switch (type) ....
      }
   }
}
