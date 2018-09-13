from glob import glob
import os


def write_to_file(str_out, dirs_set, project_name):
  # allows some automation
  cur_proj_path = f'Intermediate\\ProjectFiles\\{project_name}.vcxproj'
  with open(cur_proj_path, 'r') as proj_file:
    lines = proj_file.readlines()
  with open(cur_proj_path, 'w') as proj_file:
    for line in lines:
      split_ = line.split('<NMakeIncludeSearchPath>')
      if len(split_) > 1:
        for path in dirs_set:
          split_[1] = split_[1].replace(f'{path};', '')
        split_[1] = split_[1].replace(f'$(NMakeIncludeSearchPath);', f'$(NMakeIncludeSearchPath);{str_out}')
        proj_file.write(f'{split_[0]}<NMakeIncludeSearchPath>{split_[1]}')
      else:
        proj_file.write(line)


def get_projects(solution_dir):
  ret_val = set()
  with open(solution_dir, 'r') as solution:
    for line in solution:
      if line.startswith('Project'):
        project_name = line.split('") = "')[1].split('",')[0]
        if project_name not in ret_val:
          ret_val.add(project_name)
          ret_val.add(f'{project_name}Editor')
          ret_val.add(f'{project_name}Server')
          ret_val.add(f'{project_name}Client')
  return ret_val   

def get_include_paths(engine_root, projects):
  dirs_set = set()
  projects_include = '$(SolutionDir)\\Intermediate\\Build\\Win64\\UE4Editor\\Inc\\$(SolutionName)'
  str_out = f'{projects_include};'
  path = os.path.join(engine_root, "**\\Intermediate\\Build\\Win64", "*", "Inc\\*\\*.generated.h")
  for file in glob(path, recursive=True):
    dir_ = os.path.dirname(file)
    # use this for general paths
    # dir_ = dir_.replace(f'{engine_root}\\Engine\\', '$(MSBuildStartupDirectory)\\..\\..\\')
    if dir_ not in dirs_set:
      if dir_.split('Intermediate\\Build\\Win64\\')[1].split('\\Inc')[0] in projects:
        dirs_set.add(dir_)
  for direct in dirs_set:
    str_out += f'{direct};'
  dirs_set.add(projects_include)
  return str_out, dirs_set

def get_engine_root(project_name):
  with open(f'Intermediate\\ProjectFiles\\{project_name}.vcxproj') as file:
    for line in file:
      split_ = line.split('<NMakeBuildCommandLine>')
      if len(split_) > 1:
        return split_[1].split('\\Build\\BatchFiles\\Build.bat')[0].replace('"', '')


_solution_file = glob("*.sln")[0]
_project_name = _solution_file.split('.')[0]
_projects = get_projects(_solution_file)
_engine_root = get_engine_root(_project_name)
_str_out, _dir_set = get_include_paths(_engine_root, _projects)
print(_str_out)
write_to_file(_str_out, _dir_set, _project_name)
