#include "stdafx.h"
#include "KImageProperty.h"

int CompareEffect(ImageEffectJob* d, ImageEffectJob* s)
{
  if (d->job == s->job)
  {
    switch (d->job)
    {
    case PropertyMode::PModeRotate:
      if (d->param[0] > s->param[0])
        return 1;
      else if (d->param[0] < s->param[0])
        return -1;
      break;

    case PropertyMode::PModeColorReverse:
      if (d->param[0] > s->param[0])
        return 1;
      else if (d->param[0] < s->param[0])
        return -1;
      break;

    case PropertyMode::PModeMonochrome:
      if (d->param[0] > s->param[0])
        return 1;
      else if (d->param[0] < s->param[0])
        return -1;
      if (d->param[1] > s->param[1])
        return 1;
      else if (d->param[1] < s->param[1])
        return -1;
      break;

    case PropertyMode::PModeColorLevel:
      if (d->channel > s->channel)
        return 1;
      else if (d->channel < s->channel)
        return -1;

      for (int i = 0; i < 4; i++) // 15byte paremeters
      {
        if (d->param[i] > s->param[i])
          return 1;
        else if (d->param[i] < s->param[i])
          return -1;
      }

      break;

    case PropertyMode::PModeColorBalance:
    case PropertyMode::PModeColorHLS:
    case PropertyMode::PModeColorHSV:
      if (d->param[0] > s->param[0])
        return 1;
      else if (d->param[0] < s->param[0])
        return -1;
      if (d->param[1] > s->param[1])
        return 1;
      else if (d->param[1] < s->param[1])
        return -1;
      if (d->param[2] > s->param[2])
        return 1;
      else if (d->param[2] < s->param[2])
        return -1;
      break;

    case PropertyMode::PModeColorBright:
      if (d->param[0] > s->param[0])
        return 1;
      else if (d->param[0] < s->param[0])
        return -1;
      if (d->param[1] > s->param[1])
        return 1;
      else if (d->param[1] < s->param[1])
        return -1;
      break;

    default:
      return 0;
    }
    return 0;
  }
  return ((d->job > s->job) ? 1 : -1);

}