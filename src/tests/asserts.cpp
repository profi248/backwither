using namespace std;

/* UserInterface */
assert (UserInterface::HumanFileSize (254lu) == "254 B");
assert (UserInterface::HumanFileSize (260096lu) == "254.0 KiB");
assert (UserInterface::HumanFileSize (266338304lu) == "254.0 MiB");
assert (UserInterface::HumanFileSize (272730423296lu) == "254.00 GiB");
assert (UserInterface::HumanFileSize (279275953455104lu) == "254.00 TiB");
assert (UserInterface::HumanFileSize (2801555627573248lu) == "2.49 PiB");
assert (UserInterface::HumanFileSize (279275953455104lu) == "254.00 TiB");
assert (UserInterface::HumanFileSize (11547071114903552lu) == "10.26 PiB");
assert (UserInterface::HumanFileSize (11824200821661237248lu) == "10.26 EiB");

/* TimeUtils */
assert (TimeUtils::ParsePosColumnSeparatedInts("01:25").first == 1);
assert (TimeUtils::ParsePosColumnSeparatedInts("01:000").second == 0);
assert (TimeUtils::ParsePosColumnSeparatedInts("010000:0000100").second == 100);
assert (TimeUtils::ParsePosColumnSeparatedInts("-010000:0000100").second == 100);
assert (TimeUtils::ParsePosColumnSeparatedInts("-010000:0000100").first == -10000);

setenv("TZ", "Etc/GMT-2", 1); // actually GMT+2
tzset();
assert (TimeUtils::StringToUTCSecondsSinceStart ("00:00") == -7200);
assert (TimeUtils::StringToUTCSecondsSinceStart ("12:00") == 36000);
assert (TimeUtils::StringToUTCSecondsSinceStart ("23:59") == 75600 + 3540);
assert (TimeUtils::StringToUTCSecondsSinceStart ("24:00") == -1);
assert (TimeUtils::StringToUTCSecondsSinceStart ("10:60") == -1);

try {
    TimeUtils::StringToUTCSecondsSinceStart ("10:");
    assert (false);
} catch (invalid_argument &) {
    assert (true);
}

try {
    TimeUtils::StringToUTCSecondsSinceStart ("00:a");
    assert (false);
} catch (invalid_argument &) {
    assert (true);
}

try {
    TimeUtils::StringToUTCSecondsSinceStart (":");
    assert (false);
} catch (invalid_argument &) {
    assert (true);
}

try {
    TimeUtils::StringToUTCSecondsSinceStart (":01");
    assert (false);
} catch (invalid_argument &) {
    assert (true);
}

assert (TimeUtils::StringToWeekday("mo") == TimeUtils::MON);
assert (TimeUtils::StringToWeekday("FR") == TimeUtils::FRI);
assert (TimeUtils::StringToWeekday("Sa") == TimeUtils::SAT);
assert (TimeUtils::StringToWeekday("su") == TimeUtils::SUN);
assert (TimeUtils::StringToWeekday("adsf") == TimeUtils::NONE);

assert (TimeUtils::GetUTCTimestamp(7200) == 0);

assert (TimeUtils::PlanToString (TimeUtils::MON, -7200) == "Mon 00:00");
assert (TimeUtils::PlanToString (TimeUtils::SUN, 0) == "Sun 02:00");
assert (TimeUtils::PlanToString (TimeUtils::WED, 7200) == "Wed 04:00");