/**
* @file Layer.cs
* @date 2017.05
* @brief PageBuilder for Windows Layer class file
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PageBuilder.Data
{
    /**
    * @class Layer
    * @brief 레이어 개체 클래스
    */
    public class Layer
    {
        /** layer-visible 속성을 나타내는 상수값 */
        public const int LAYER_VISIBLE = 1;
        /** layer-locked 속성을 나타내는 상수값 */
        public const int LAYER_LOCKED = 2;
        /** 유일한 레이어 ID값을 만들기 위한 static 변수*/
        static int MaxLayerId = 1;

        /** 레이어 이름 */
        public string Name;
        /** 레이어 속성(visible, locked) */
        public int Attribute;
        /** 레이어 ID */
        public int Id;

        /**
        * @brief Layer class constructor
        */
        public Layer()
        {
        }

        /**
        * @brief Layer class constructor
        * @param name : 레이어 이름
        */
        public Layer(string name)
        {
            Name = name;
            Attribute = LAYER_VISIBLE;
            Id = MaxLayerId;
            MaxLayerId++;
        }

        /**
        * @brief 레이어의 ID값을 리턴한다.
        */
        public int GetId()
        {
            return Id;
        }

        /**
        * @brief 레이어가 visible하면 true를 리턴한다.
        */
        public bool IsLayerVisible()
        {
            return ((Attribute & LAYER_VISIBLE) == LAYER_VISIBLE);
        }

        /**
        * @brief 레이어의 visible 속성을 설정 또는 해제한다.
        */
        public bool SetLayerVisible(bool visible)
        {
            if (visible && ((Attribute & LAYER_VISIBLE) == 0))
            {
                Attribute |= LAYER_VISIBLE;
                return true;
            }
            else if ((visible == false) && ((Attribute & LAYER_VISIBLE) == LAYER_VISIBLE))
            {
                Attribute &= ~LAYER_VISIBLE;
                return true;
            }
            return false;
        }

        /**
        * @brief 레이어가 잠금 상태이면 true를 리턴한다.
        */
        public bool IsLayerLocked()
        {
            return ((Attribute & LAYER_LOCKED) == LAYER_LOCKED);
        }

        /**
        * @brief 레이어의 잠금 속성을 설정 또는 해제한다.
        */
        public bool SetLayerLock(bool locked)
        {
            if (locked && ((Attribute & LAYER_LOCKED) == 0))
            {
                Attribute |= LAYER_LOCKED;
                return true;
            }
            else if ((locked == false) && ((Attribute & LAYER_LOCKED) == LAYER_LOCKED))
            {
                Attribute &= ~LAYER_LOCKED;
                return true;
            }
            return false;
        }
    }
}
