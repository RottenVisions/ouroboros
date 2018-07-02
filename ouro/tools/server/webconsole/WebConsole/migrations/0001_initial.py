# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='AuthUser',
            fields=[
                ('id', models.AutoField(serialize=False, primary_key=True)),
                ('name', models.CharField(default='', unique=True, db_index=True, help_text='Account', max_length=128)),
                ('show_name', models.CharField(default='', help_text='Display name', max_length=128)),
                ('password', models.CharField(default='', help_text='Password', max_length=128)),
                ('sys_user', models.CharField(default='', help_text='System account', max_length=128)),
                ('sys_uid', models.IntegerField(default=0, help_text='System account ID')),
            ],
        ),
        migrations.CreateModel(
            name='ServerLayout',
            fields=[
                ('id', models.AutoField(serialize=False, primary_key=True)),
                ('name', models.CharField(default='', unique=True, db_index=True, help_text='Name', max_length=128)),
                ('sys_user', models.CharField(default='', help_text='System account', max_length=128)),
                ('config', models.TextField(default='', help_text='Configuration(JSON)', max_length=32768)),
            ],
        ),
    ]
