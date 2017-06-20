bl_info = {
    "name": ".lavy exporter",
    "category": "Import-Export",
}

import os
import bpy
import json
import bmesh
import ctypes
from mathutils import Vector
from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper
from shutil import copyfile

class Vertex:
    __slots__ = (
        "pos",
        "normal",
        "uv",
        "index",
        "loop_indices"
    )
    
    def __init__(self, mesh, loop):
        self.pos = mesh.vertices[loop.vertex_index].co.freeze()
        self.normal = loop.normal.freeze()
        self.uv = mesh.uv_layers[0].data[loop.index].uv.freeze()
        self.loop_indices = [loop.index]
        self.index = 0

    def __hash__(self):
        return hash((self.pos, self.normal, self.uv))
    
    def __eq__(self, other):
        eq = (
            (self.pos == other.pos) and
            (self.normal == other.normal) and
            (self.uv == other.uv)
        )

        if eq:
            indices = self.loop_indices + other.loop_indices
            self.loop_indices = indices
            other.loop_indices = indices

        return eq
  
class LavyExporter(bpy.types.Operator, ExportHelper):
    bl_idname = "export_mesh.lavy"
    bl_label = "Export lavy"
    filename_ext = ".lavy"
    use_filter_folder = True
    directory = None
    
    def export_meshes(self, meshes , export_bin):
        offset = 0
        last_total = 0
        mesh_table = dict()
        for mesh_obj in meshes:
            texture_index = 0
            textures = list()
            for material_slot in mesh_obj.material_slots:
                for texture_slot in material_slot.material.texture_slots:
                    if texture_slot == None:
                        continue
                    if texture_slot.texture.type == 'IMAGE':
                        ext = texture_slot.texture.image.file_format
                        filename = texture_slot.texture.image.name
                        textures.append(filename)
                        copyfile(bpy.path.abspath(texture_slot.texture.image.filepath), self.directory + filename)
                        texture_index = texture_index + 1
            mesh = mesh_obj.data
            mesh.calc_normals_split()
            mesh.calc_tessface()

            verts = { Vertex(mesh, loop) : 0 for loop in mesh.loops }.keys()
            verts_count = len(verts)
            
            for i, vert in enumerate(verts):
                max_x = vert.pos.y
                max_y = vert.pos.z
                max_z = vert.pos.x
                min_x = vert.pos.y
                min_y = vert.pos.z
                min_z = vert.pos.x
                break
            
            for i, vert in enumerate(verts):
                vert.index = i
                export_bin.write(ctypes.c_float(vert.pos.y))
                export_bin.write(ctypes.c_float(vert.pos.z))
                export_bin.write(ctypes.c_float(vert.pos.x))
                export_bin.write(ctypes.c_float(vert.normal.y))
                export_bin.write(ctypes.c_float(vert.normal.z))
                export_bin.write(ctypes.c_float(vert.normal.x))
                uv_x = vert.uv.x
                uv_y = vert.uv.y
                export_bin.write(ctypes.c_float(uv_x))
                export_bin.write(ctypes.c_float(1 - uv_y))
                offset += 32
                if vert.pos.y > max_x :
                    max_x = vert.pos.y
                if vert.pos.z > max_y :
                    max_y = vert.pos.z
                if vert.pos.x > max_z :
                    max_z = vert.pos.x
                if vert.pos.y < min_x :
                    min_x = vert.pos.y
                if vert.pos.z < min_y :
                    min_y = vert.pos.z
                if vert.pos.x < min_z :
                    min_z = vert.pos.x
            vertices_byte_length = offset
            vert_dict = { i : v for v in verts for i in v.loop_indices}

            for poly in mesh.polygons:
                indices = [vert_dict[i].index for i in poly.loop_indices]
                if len(indices) == 3:
                    for i in indices:
                        c_i = ctypes.c_uint32(i)
                        export_bin.write(c_i)
                        offset += 4
                elif len(indices) > 3:
                    for i in range(len(indices) - 2):
                        c_i1 = ctypes.c_uint32(indices[-1])
                        c_i2 = ctypes.c_uint32(indices[i])
                        c_i3 = ctypes.c_uint32(indices[i + 1])
                        export_bin.write(c_i1)
                        export_bin.write(c_i2)
                        export_bin.write(c_i3)
                        offset += 12
            
            indices_byte_length = offset - vertices_byte_length
            
            mesh_table[mesh_obj.name] = {
                'vertices': {
                    'byteLength': vertices_byte_length,
                    'byteOffset': last_total
                },
                'indices': {
                    'byteLength': indices_byte_length,
                    'byteOffset': last_total + vertices_byte_length
                },
                'textures': textures,
                'bounding_box' : {
                    'min': [min_x, min_y, min_z],
                    'max': [max_x, max_y, max_z]
                }
            }
            
            last_total = last_total + vertices_byte_length + indices_byte_length

        return mesh_table;

    def execute(self, context):
        self.directory = os.path.dirname(self.filepath) + "/"
        if bpy.context.active_object.mode!='OBJECT':
            bpy.ops.object.mode_set(mode='OBJECT')
        scene = bpy.context.scene
        
        if len(bpy.context.selected_objects)>0:
            exporting_obj = bpy.context.selected_objects[0]
            export_json = open(self.filepath + ".json", "w")
            export_bin = open(self.filepath + ".bin", "wb")
            
            j_data = None
            if exporting_obj.type == 'MESH':
                meshes = [exporting_obj]
                j_data = json.dumps(
                    {
                        'meshes' : self.export_meshes(meshes, export_bin)
                    }
                )
            elif exporting_obj.type == 'ARMATURE':
                j_data = json.dumps(
                    {
                        'meshes' : self.export_meshes(exporting_obj.children, export_bin)
                    }
                )
            
            export_json.write(j_data)
            export_json.close()
            export_bin.close()
            
        return {'FINISHED'}

def menu_export(self, context):
    self.layout.operator(LavyExporter.bl_idname, text="Lavy")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()