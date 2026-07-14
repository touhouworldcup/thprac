  void tmp() {
    // from 42c655, 4bytes

    bVar12 = param_1 / 1000000 != 0;
    if (bVar12) {
      FUN_00404b80(*(void **)(g_PhotoManager + 0x2571c),(undefined4 *)((int)this + 0x245c),0x1e);
      FUN_00439e30(*(void **)(g_PhotoManager + 0x2571c),(int)this + 0x245c,param_1 / 1000000 + 0xf);
    }
    *(float *)((int)this + 0x25b0) = fVar4 - 9.0;
    *(float *)((int)this + 0x25b4) = fVar5;
    *(float *)((int)this + 0x25b8) = fVar6;

    iVar8 = (param_1 / 100000) % 10;
    if ((iVar8 != 0) || (bVar12)) {
      FUN_00404b80(*(void **)(g_PhotoManager + 0x2571c),(undefined4 *)((int)this + 0x10c8),0x1e);
      FUN_00439e30(*(void **)(g_PhotoManager + 0x2571c),(int)this + 0x10c8,iVar8 + 0xf);
      bVar12 = true;
    }
    // 42c6a5
  }