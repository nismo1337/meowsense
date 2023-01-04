#pragma once
namespace OW {
	static Vector2 GetAimEnemy()
	{
		int TarGetIndex = -1;
		Vector2 target = Vector2(0, 0);
		Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);

		float origin = FLT_MAX;

		if (TarGetIndex == -1)
		{
			if (Entity_t.size() > 0)
			{
				for (int i = 0; i < Entity_t.size(); i++)
				{
					if (Entitys[i].Live && Entitys[i].Team)
					{
						Vector2 Vec2{};
						if (!viewMatrix.WorldToScreen(Entitys[i].Loc, &Vec2))
							continue;

						Vector2 RealVe2 = Vector2(Vec2.X - CrossHair.X, Vec2.Y - CrossHair.Y);
						float CrossDist = CrossHair.Distance(Vec2);

						if (CrossDist < origin && CrossDist < Config::Fov)
						{
							target = RealVe2;
							origin = CrossDist;
							TarGetIndex = i;
						}
						else
						{
							TarGetIndex = -1;
						}
					}
					else
					{
						TarGetIndex = -1;
					}

				}
			}
		}
		else
		{
			if (Entitys[TarGetIndex].Live && Entitys[TarGetIndex].Team)
			{
				Vector2 Vec2{};
				if (!viewMatrix.WorldToScreen(Entitys[TarGetIndex].Loc, &Vec2))
					return {0 , 0};

				Vector2 RealVe2 = Vector2(Vec2.X - CrossHair.X, Vec2.Y - CrossHair.Y);
				float CrossDist = CrossHair.Distance(Vec2);

				if (CrossDist < origin && CrossDist < Config::Fov)
				{
					target = RealVe2;
					origin = CrossDist;
				}
				else
				{
					TarGetIndex = -1;

				}
			}
			else
			{
				TarGetIndex = -1;
			}
		}

		return target;
	}
}