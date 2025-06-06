-- Author: Owsap
-- Filename: GuildDragonLair.lua

GuildDragonLairSetting = {}

GuildDragonLairSetting.StoneCount = 4 -- 소스에서 고정된 값입니다!
GuildDragonLairSetting.EggCount = 4
GuildDragonLairSetting.BossCount = 4

GuildDragonLairSetting.Event = {}
GuildDragonLairSetting.Event.OpenGate = 3 -- 문이 부서질 때까지의 대기 시간.
GuildDragonLairSetting.Event.Expire = 3600 -- 던전이 끝날 때까지의 대기 시간.
GuildDragonLairSetting.Event.ExitSuccess = 600 --성공적으로 던전을 나갈 때 대기 시간.
GuildDragonLairSetting.Event.ExitFailure = 60 -- 실패할 경우 던전을 나갈 때 대기 시간.
GuildDragonLairSetting.Event.SpecialAttack = 5 -- 동상이 공격할 때까지의 대기 시간.
GuildDragonLairSetting.Event.RestAfterSpecialAttack = 30 -- 동상이 공격한 후의 대기 시간.

GuildDragonLairSetting.PixelPosition = {}
GuildDragonLairSetting.PixelPosition.Outside = {}
GuildDragonLairSetting.PixelPosition.Outside.x = 84
GuildDragonLairSetting.PixelPosition.Outside.y = 850
GuildDragonLairSetting.PixelPosition.Outside.z = 0
GuildDragonLairSetting.PixelPosition.Outside.dir = 0
GuildDragonLairSetting.PixelPosition.OutsideWatcher = {}
GuildDragonLairSetting.PixelPosition.OutsideWatcher.x = 135
GuildDragonLairSetting.PixelPosition.OutsideWatcher.y = 179
GuildDragonLairSetting.PixelPosition.OutsideWatcher.z = 0
GuildDragonLairSetting.PixelPosition.OutsideWatcher.dir = 8
GuildDragonLairSetting.PixelPosition.InsideWatcher = {}
GuildDragonLairSetting.PixelPosition.InsideWatcher.x = 134
GuildDragonLairSetting.PixelPosition.InsideWatcher.y = 168
GuildDragonLairSetting.PixelPosition.InsideWatcher.z = 0
GuildDragonLairSetting.PixelPosition.InsideWatcher.dir = 6
GuildDragonLairSetting.PixelPosition.Inside = {}
GuildDragonLairSetting.PixelPosition.Inside.x = 127
GuildDragonLairSetting.PixelPosition.Inside.y = 167
GuildDragonLairSetting.PixelPosition.Inside.z = 0
GuildDragonLairSetting.PixelPosition.Inside.dir = 0
GuildDragonLairSetting.PixelPosition.Gate = {}
GuildDragonLairSetting.PixelPosition.Gate.x = 129
GuildDragonLairSetting.PixelPosition.Gate.y = 175
GuildDragonLairSetting.PixelPosition.Gate.z = 0
GuildDragonLairSetting.PixelPosition.Gate.dir = 5
GuildDragonLairSetting.PixelPosition.Center = {}
GuildDragonLairSetting.PixelPosition.Center.x = 130
GuildDragonLairSetting.PixelPosition.Center.y = 130
GuildDragonLairSetting.PixelPosition.Center.z = 0
GuildDragonLairSetting.PixelPosition.Center.dir = 1
GuildDragonLairSetting.PixelPosition.King = {}
GuildDragonLairSetting.PixelPosition.King.x = 130
GuildDragonLairSetting.PixelPosition.King.y = 77
GuildDragonLairSetting.PixelPosition.King.z = 0
GuildDragonLairSetting.PixelPosition.King.dir = 1
GuildDragonLairSetting.PixelPosition.Stone = {}
GuildDragonLairSetting.PixelPosition.Stone[1] = {}
GuildDragonLairSetting.PixelPosition.Stone[1].x = 123
GuildDragonLairSetting.PixelPosition.Stone[1].y = 137
GuildDragonLairSetting.PixelPosition.Stone[1].z = 0
GuildDragonLairSetting.PixelPosition.Stone[1].dir = 5
GuildDragonLairSetting.PixelPosition.Stone[2] = {}
GuildDragonLairSetting.PixelPosition.Stone[2].x = 123
GuildDragonLairSetting.PixelPosition.Stone[2].y = 124
GuildDragonLairSetting.PixelPosition.Stone[2].z = 0
GuildDragonLairSetting.PixelPosition.Stone[2].dir = 5
GuildDragonLairSetting.PixelPosition.Stone[3] = {}
GuildDragonLairSetting.PixelPosition.Stone[3].x = 136
GuildDragonLairSetting.PixelPosition.Stone[3].y = 123
GuildDragonLairSetting.PixelPosition.Stone[3].z = 0
GuildDragonLairSetting.PixelPosition.Stone[3].dir = 5
GuildDragonLairSetting.PixelPosition.Stone[4] = {}
GuildDragonLairSetting.PixelPosition.Stone[4].x = 137
GuildDragonLairSetting.PixelPosition.Stone[4].y = 137
GuildDragonLairSetting.PixelPosition.Stone[4].z = 0
GuildDragonLairSetting.PixelPosition.Stone[4].dir = 5

GuildDragonLairSetting.Guild = {}
GuildDragonLairSetting.Guild.PlayerLevel = 90
GuildDragonLairSetting.Guild.GuildLevel = 3
GuildDragonLairSetting.Guild.LadderPoint = 600
GuildDragonLairSetting.Guild.MinMember = 4
GuildDragonLairSetting.Guild.MaxMember = 16
GuildDragonLairSetting.Guild.WaitTime = 10800
GuildDragonLairSetting.Guild.TicketGroup = 10037
GuildDragonLairSetting.Guild.RankingShowLimit = 10
GuildDragonLairSetting.Guild.WatcherVnum = 20419 -- 의천
GuildDragonLairSetting.Guild.GateVnum = 20388 -- 적룡결계문
GuildDragonLairSetting.Guild.KingVnum = 6193 -- 적룡
GuildDragonLairSetting.Guild.StoneVnum = 6118 -- 드래곤석상
GuildDragonLairSetting.Guild.EggVnum = 20422 -- 적룡석
GuildDragonLairSetting.Guild.BossVnum = 6116 -- 인간형보스
GuildDragonLairSetting.Guild.KnightVnum = 6117 -- 부활한인간형
GuildDragonLairSetting.Guild.ChestVnum = 20420 -- 용방보상
GuildDragonLairSetting.Guild.Stage = {}
GuildDragonLairSetting.Guild.Stage[1] = {}
GuildDragonLairSetting.Guild.Stage[1].StoneHealthPct = 75
GuildDragonLairSetting.Guild.Stage[1].MonsterVnum = 6112 -- 극열정+
GuildDragonLairSetting.Guild.Stage[1].MonsterCount = 12
GuildDragonLairSetting.Guild.Stage[1].MonsterRespawnTime = 5
GuildDragonLairSetting.Guild.Stage[1].EggRespawnTime = 120
GuildDragonLairSetting.Guild.Stage[1].DropPct = 5
GuildDragonLairSetting.Guild.Stage[2] = {}
GuildDragonLairSetting.Guild.Stage[2].StoneHealthPct = 50
GuildDragonLairSetting.Guild.Stage[2].MonsterVnum = 6113 -- 혈화정+
GuildDragonLairSetting.Guild.Stage[2].MonsterCount = 12
GuildDragonLairSetting.Guild.Stage[2].MonsterRespawnTime = 30
GuildDragonLairSetting.Guild.Stage[2].EggRespawnTime = 120
GuildDragonLairSetting.Guild.Stage[2].DropPct = 5
GuildDragonLairSetting.Guild.Stage[3] = {}
GuildDragonLairSetting.Guild.Stage[3].StoneHealthPct = 1
GuildDragonLairSetting.Guild.Stage[3].MonsterVnum = 6115 -- 새끼적룡
GuildDragonLairSetting.Guild.Stage[3].MonsterCount = 12
GuildDragonLairSetting.Guild.Stage[3].MonsterRespawnTime = 30
GuildDragonLairSetting.Guild.Stage[3].EggRespawnTime = 120
GuildDragonLairSetting.Guild.Stage[3].DropPct = 1
GuildDragonLairSetting.Guild.Stage[4] = {}
GuildDragonLairSetting.Guild.Stage[4].StoneHealthPct = 1
GuildDragonLairSetting.Guild.Stage[4].MonsterVnum = 6115 -- 새끼적룡
GuildDragonLairSetting.Guild.Stage[4].MonsterCount = 12
GuildDragonLairSetting.Guild.Stage[4].MonsterRespawnTime = 30
GuildDragonLairSetting.Guild.Stage[4].EggRespawnTime = 120
GuildDragonLairSetting.Guild.Stage[4].DropPct = 1

GuildDragonLairSetting.Party = {}
GuildDragonLairSetting.Party.PlayerLevel = 75
GuildDragonLairSetting.Party.MinMember = 2
GuildDragonLairSetting.Party.MaxMember = 8
GuildDragonLairSetting.Party.WaitTime = 10800
GuildDragonLairSetting.Party.TicketGroup = 10034
GuildDragonLairSetting.Party.WatcherVnum = 20421 -- 의천2
GuildDragonLairSetting.Party.GateVnum = 20388 -- 적룡결계문
GuildDragonLairSetting.Party.KingVnum = 6193 -- 적룡
GuildDragonLairSetting.Party.StoneVnum = 6209 -- 드래곤석상
GuildDragonLairSetting.Party.EggVnum = 20500 -- 적룡석
GuildDragonLairSetting.Party.BossVnum = 6207 -- 인간형보스
GuildDragonLairSetting.Party.KnightVnum = 6117 -- 부활한인간형
GuildDragonLairSetting.Party.ChestVnum = 20501 -- 용방보상(p)
GuildDragonLairSetting.Party.Stage = {}
GuildDragonLairSetting.Party.Stage[1] = {}
GuildDragonLairSetting.Party.Stage[1].StoneHealthPct = 75
GuildDragonLairSetting.Party.Stage[1].MonsterVnum = 6203 -- 극열정+
GuildDragonLairSetting.Party.Stage[1].MonsterCount = 12
GuildDragonLairSetting.Party.Stage[1].MonsterRespawnTime = 5
GuildDragonLairSetting.Party.Stage[1].EggRespawnTime = 120
GuildDragonLairSetting.Party.Stage[1].DropPct = 15
GuildDragonLairSetting.Party.Stage[2] = {}
GuildDragonLairSetting.Party.Stage[2].StoneHealthPct = 50
GuildDragonLairSetting.Party.Stage[2].MonsterVnum = 6204 -- 혈화정+
GuildDragonLairSetting.Party.Stage[2].MonsterCount = 12
GuildDragonLairSetting.Party.Stage[2].MonsterRespawnTime = 30
GuildDragonLairSetting.Party.Stage[2].EggRespawnTime = 120
GuildDragonLairSetting.Party.Stage[2].DropPct = 10
GuildDragonLairSetting.Party.Stage[3] = {}
GuildDragonLairSetting.Party.Stage[3].StoneHealthPct = 1
GuildDragonLairSetting.Party.Stage[3].MonsterVnum = 6206 -- 새끼적룡
GuildDragonLairSetting.Party.Stage[3].MonsterCount = 12
GuildDragonLairSetting.Party.Stage[3].MonsterRespawnTime = 30
GuildDragonLairSetting.Party.Stage[3].EggRespawnTime = 120
GuildDragonLairSetting.Party.Stage[3].DropPct = 5
GuildDragonLairSetting.Party.Stage[4] = {}
GuildDragonLairSetting.Party.Stage[4].StoneHealthPct = 1
GuildDragonLairSetting.Party.Stage[4].MonsterVnum = 6206 -- 새끼적룡
GuildDragonLairSetting.Party.Stage[4].MonsterCount = 12
GuildDragonLairSetting.Party.Stage[4].MonsterRespawnTime = 30
GuildDragonLairSetting.Party.Stage[4].EggRespawnTime = 120
GuildDragonLairSetting.Party.Stage[4].DropPct = 5
