// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_TESTS_VECTOR2_GUARD
#define SAPPHIRE_TESTS_VECTOR2_GUARD

#include "../../UnitTest.hpp"

#include <Sapphire/Core/Misc/Random.hpp>
#include <Sapphire/Maths/Misc/Degree.hpp>
#include <Sapphire/Maths/Misc/Radian.hpp>
#include <Sapphire/Maths/Space/Vector2.hpp>

#define LOG_V(_v) LOG(#_v ": " << _v)

namespace Sa
{
	std::ostream& operator<<(std::ostream& _stream, const Vec2d& _v)
	{
		_stream << '(' << _v.x << ',' << _v.y << ')';

		return _stream;
	}

	Vec2d GenerateRandVector()
	{
		return Vec2d(Random<double>::Value(-100.0, 100.0), Random<double>::Value(-100.0, 100.0));
	}

	void Test()
	{
		std::cout.precision(7);

		LOG("\n=== Constructors ===");
		{
			const double constr_v1X = Random<double>::Value(-100.0, 100.0);
			const double constr_v1Y = Random<double>::Value(-100.0, 100.0);
			const Vec2d constr_v1(constr_v1X, constr_v1Y); LOG_V(constr_v1);

			SA_TEST(constr_v1.x, == , constr_v1X);
			SA_TEST(constr_v1.y, == , constr_v1Y);


			const double constr_v2S = Random<double>::Value(-100.0f, 100.0f);
			const Vec2d constr_v2(constr_v2S); LOG_V(constr_v2);

			SA_TEST(constr_v2.x, == , constr_v2S);
			SA_TEST(constr_v2.y, == , constr_v2S);


			const int32 constr_v3X = Random<int32>::Value(-100, 100);
			const int32 constr_v3Y = Random<int32>::Value(-100, 100);
			const Vec2d constr_v3(Vec2i(constr_v3X, constr_v3Y)); LOG_V(constr_v3);

			SA_TEST(constr_v3.x, == , static_cast<double>(constr_v3X));
			SA_TEST(constr_v3.y, == , static_cast<double>(constr_v3Y));


			const Vec2d constr_v4(constr_v1); LOG_V(constr_v4);

			SA_TEST(constr_v4.x, == , constr_v1.x);
			SA_TEST(constr_v4.y, == , constr_v1.y);
		}


		LOG("\n=== Equal ===");
		{
			const Vec2d eq_v1 = GenerateRandVector(); LOG_V(eq_v1);
			const Vec2d eq_v2 = GenerateRandVector(); LOG_V(eq_v2);

			SA_TEST(eq_v1.IsZero(), == , false);
			SA_TEST(Vec2d::Zero.IsZero(), == , true);

			SA_TEST(eq_v1.Equals(eq_v1), == , true);
			SA_TEST(eq_v1.Equals(eq_v2), == , false);

			SA_TEST(eq_v1, == , eq_v1);
			SA_TEST(eq_v1, != , eq_v2);
		}


		LOG("\n=== Length ===");
		for (uint32 i = 0u; i < UnitTest::TestNum; ++i)
		{
			Vec2d len_v = GenerateRandVector(); LOG_V(len_v);

			const double len2_vd = len_v.x * len_v.x + len_v.y * len_v.y;
			const double len_vd = Maths::Sqrt(len2_vd);
			SA_TEST(len_v.Length(), == , len_vd);
			SA_TEST(len_v.SqrLength(), == , len2_vd);

			const Vec2d nLen_v = len_v.GetNormalized(); LOG_V(nLen_v);
			SA_TEST(nLen_v.x, == , len_v.x / len_vd);
			SA_TEST(nLen_v.y, == , len_v.y / len_vd);

			SA_TEST(nLen_v.IsNormalized(), == , true);
			SA_TEST(len_v.IsNormalized(), == , false);

			len_v.Normalize();
			SA_TEST(len_v.x, == , nLen_v.x);
			SA_TEST(len_v.y, == , nLen_v.y);
		}


		LOG("\n=== Reflect ===");
		{
			const Vec2d rfl_v1(1.0f, 1.0f); LOG_V(rfl_v1);
			const Vec2d rfl_norm(-1.0f, 0.0f); LOG_V(rfl_norm);

			const Vec2d rfl_v2 = rfl_v1.Reflect(rfl_norm);
			SA_TEST(rfl_v2.x, == , -1.0f);
			SA_TEST(rfl_v2.y, == , 1.0f);
		}


		LOG("\n=== Dot /Cross ===");
		for (uint32 i = 0u; i < UnitTest::TestNum; ++i)
		{
			const Vec2d dc_v1 = GenerateRandVector(); LOG_V(dc_v1);
			const Vec2d dc_v2 = GenerateRandVector(); LOG_V(dc_v2);

			SA_TEST(Vec2d::Dot(dc_v1, dc_v2), == , (dc_v1.x * dc_v2.x + dc_v1.y * dc_v2.y));
			SA_TEST(Vec2d::Cross(dc_v1, dc_v2), == , (dc_v1.x * dc_v2.y + dc_v1.y * dc_v2.x));
		}


		LOG("\n=== Angle ===");
		{
			const Vec2d angl_v1(-2.0, 1.0); LOG_V(angl_v1);
			const Vec2d angl_v2(1.0, 2.0); LOG_V(angl_v2);

			SA_TEST(Vec2d::Angle(angl_v1, angl_v2), == , -90.0);
			SA_TEST(Vec2d::AngleUnsigned(angl_v1, angl_v2), == , 90.0);
		}


		LOG("\n=== Dist ===");
		{
			const Vec2d dist_v1 = GenerateRandVector(); LOG_V(dist_v1);
			const Vec2d dist_v2 = GenerateRandVector(); LOG_V(dist_v2);

			SA_TEST(Vec2d::Dist(dist_v1, dist_v2), == , (dist_v1 - dist_v2).Length());
			SA_TEST(Vec2d::SqrDist(dist_v1, dist_v2), == , (dist_v1 - dist_v2).SqrLength());
		}


		LOG("\n=== Dir ===");
		for (uint32 i = 0u; i < UnitTest::TestNum; ++i)
		{
			const Vec2d dir_v1 = GenerateRandVector(); LOG_V(dir_v1);
			const Vec2d dir_v2 = GenerateRandVector(); LOG_V(dir_v2);

			const double dirX = dir_v2.x - dir_v1.x;
			const double dirY = dir_v2.y - dir_v1.y;
			const double dirLen = Maths::Sqrt(dirX * dirX + dirY * dirY);

			SA_TEST(Vec2d::Dir(dir_v1, dir_v2).x, == , dirX);
			SA_TEST(Vec2d::Dir(dir_v1, dir_v2).y, == , dirY);

			SA_TEST(Vec2d::DirN(dir_v1, dir_v2).x, == , dirX / dirLen);
			SA_TEST(Vec2d::DirN(dir_v1, dir_v2).y, == , dirY / dirLen);
		}


		LOG("\n=== Lerp / SLerp ===");
		{
			const Vec2f lerp_v1(2.0f, 2.0f); LOG_V(lerp_v1);
			const Vec2f lerp_v2(-2.0f, 4.0f); LOG_V(lerp_v2);

			const Vec2f lerp_res05 = Vec2f::Lerp(lerp_v1, lerp_v2, 0.5f);
			SA_TEST(lerp_res05.x, == , 0.0f);
			SA_TEST(lerp_res05.y, == , 3.0f);

			const Vec2f lerp_res2 = Vec2f::Lerp(lerp_v1, lerp_v2, 2.0f);
			SA_TEST(lerp_res2.x, == , lerp_v2.x);
			SA_TEST(lerp_res2.y, == , lerp_v2.y);

			const Vec2f ulerp_res1 = Vec2f::LerpUnclamped(lerp_v1, lerp_v2, -1.0f);
			SA_TEST(ulerp_res1.x, == , 6.0f);
			SA_TEST(ulerp_res1.y, == , 0.0f);


			const Vec2f slerp_v1(2.0f, 2.0f); LOG_V(slerp_v1);
			const Vec2f slerp_v2(-2.0f, 2.0f); LOG_V(slerp_v2);
			const Vec2f slerp_res05 = Vec2f::SLerp(slerp_v1, slerp_v2, 0.5f);
			SA_TEST(slerp_res05.x, == , 0.0f);
			SA_TEST(slerp_res05.y, == , slerp_v1.Length());
		}


		LOG("\n=== Operator ===");
		{
			const Vec2d op_v1 = GenerateRandVector(); LOG_V(op_v1);

			SA_TEST(-op_v1.x, == , -op_v1.x);
			SA_TEST(-op_v1.y, == , -op_v1.y);

			double op_scale = Random<double>::Value(-100.0, 100.0); LOG("Scale: " << op_scale);
			SA_TEST((op_v1 * op_scale).x, == , op_v1.x * op_scale);
			SA_TEST((op_v1 * op_scale).y, == , op_v1.y * op_scale);

			SA_TEST((op_scale * op_v1).x, == , op_v1.x * op_scale);
			SA_TEST((op_scale * op_v1).y, == , op_v1.y * op_scale);

			SA_TEST((op_v1 / op_scale).x, == , op_v1.x / op_scale);
			SA_TEST((op_v1 / op_scale).y, == , op_v1.y / op_scale);

			SA_TEST((op_scale / op_v1).x, == , op_scale / op_v1.x);
			SA_TEST((op_scale / op_v1).y, == , op_scale / op_v1.y);

			const Vec2d op_v2 = GenerateRandVector(); LOG_V(op_v2);
			SA_TEST((op_v1 + op_v2).x, == , op_v1.x + op_v2.x);
			SA_TEST((op_v1 + op_v2).y, == , op_v1.y + op_v2.y);

			SA_TEST((op_v1 - op_v2).x, == , op_v1.x - op_v2.x);
			SA_TEST((op_v1 - op_v2).y, == , op_v1.y - op_v2.y);

			SA_TEST((op_v1 * op_v2).x, == , op_v1.x * op_v2.x);
			SA_TEST((op_v1 * op_v2).y, == , op_v1.y * op_v2.y);

			SA_TEST((op_v1 / op_v2).x, == , op_v1.x / op_v2.x);
			SA_TEST((op_v1 / op_v2).y, == , op_v1.y / op_v2.y);

			Vec2d op_v3 = op_v1;
			op_v3 *= op_scale;
			SA_TEST(op_v3.x, == , op_v1.x * op_scale);
			SA_TEST(op_v3.y, == , op_v1.y * op_scale);

			Vec2d op_v4 = op_v1;
			op_v4 /= op_scale;
			SA_TEST(op_v4.x, == , op_v1.x / op_scale);
			SA_TEST(op_v4.y, == , op_v1.y / op_scale);

			Vec2d op_v5 = op_v1;
			op_v5 += op_v2;
			SA_TEST(op_v5.x, == , op_v1.x + op_v2.x);
			SA_TEST(op_v5.y, == , op_v1.y + op_v2.y);

			Vec2d op_v6 = op_v1;
			op_v6 -= op_v2;
			SA_TEST(op_v6.x, == , op_v1.x - op_v2.x);
			SA_TEST(op_v6.y, == , op_v1.y - op_v2.y);

			Vec2d op_v7 = op_v1;
			op_v7 *= op_v2;
			SA_TEST(op_v7.x, == , op_v1.x * op_v2.x);
			SA_TEST(op_v7.y, == , op_v1.y * op_v2.y);

			Vec2d op_v8 = op_v1;
			op_v8 /= op_v2;
			SA_TEST(op_v8.x, == , op_v1.x / op_v2.x);
			SA_TEST(op_v8.y, == , op_v1.y / op_v2.y);
		}


		LOG("\n=== Operator [] ===");
		{
			const Vec2d opacc_v1 = GenerateRandVector(); LOG_V(opacc_v1);

			SA_TEST(opacc_v1[0], == , opacc_v1.x);
			SA_TEST(opacc_v1[1], == , opacc_v1.y);
		}
	}
}

#endif // GUARD
